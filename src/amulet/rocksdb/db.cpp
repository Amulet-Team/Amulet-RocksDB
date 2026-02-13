#include <filesystem>
#include <memory>
#include <set>
#include <string>
#include <string_view>

#include <rocksdb/db.h>

#include "compact_range_options.hpp"
#include "db.hpp"
#include "iterator.hpp"
#include "options.hpp"
#include "read_options.hpp"
#include "write_options.hpp"

namespace Amulet {
namespace RocksDB {

    struct RocksDBImpl {
        ROCKSDB_NAMESPACE::DB* db;
        ROCKSDB_NAMESPACE::ReadOptions read_options;
        ROCKSDB_NAMESPACE::WriteOptions write_options;
        ROCKSDB_NAMESPACE::CompactRangeOptions compact_range_options;
        std::set<Iterator*> iterators;
        std::recursive_mutex iterators_mutex;
    };

    static void remove_iterator(void* _impl_voidp, void* it_voidp)
    {
        auto* _impl = reinterpret_cast<RocksDBImpl*>(_impl_voidp);
        auto* it = reinterpret_cast<Iterator*>(it_voidp);
        std::lock_guard lock(_impl->iterators_mutex);
        _impl->iterators.erase(it);
    }

    RocksDB::RocksDB(
        std::filesystem::path path,
        Options&& options,
        ReadOptions&& read_options,
        WriteOptions&& write_options,
        CompactRangeOptions&& compact_range_options)
    {
        // Expand dots and symbolic links
        path = std::filesystem::absolute(path);

        std::unique_ptr<ROCKSDB_NAMESPACE::DB> db = nullptr;
        auto status = ROCKSDB_NAMESPACE::DB::Open(*options.borrow(), path.string(), &db);
        if (status.IsCorruption()) {
            ROCKSDB_NAMESPACE::RepairDB(path.string(), *options.borrow());
            status = ROCKSDB_NAMESPACE::DB::Open(*options.borrow(), path.string(), &db);
        }
        if (!status.ok()) {
            throw RocksDBException(status.ToString());
        }
        _impl = new RocksDBImpl {
            db.release(),
            *read_options.steal(),
            *write_options.steal(),
            *compact_range_options.steal()
        };
    }

    RocksDB::RocksDB(
        std::filesystem::path path,
        bool create_if_missing,
        CompressionType compression_type)
        : RocksDB(
              path,
              [create_if_missing, compression_type]() {
                  Options options;
                  options.set_create_if_missing(create_if_missing);
                  options.set_compression_type(compression_type);
                  return options;
              }(),
              ReadOptions(),
              WriteOptions(),
              CompactRangeOptions())
    {
        Options options;
        ReadOptions read_options;
        WriteOptions write_options;
        /*auto options = std::make_unique<LevelDBOptions>();
        options->options.create_if_missing = create_if_missing;
        options->options.filter_policy = ROCKSDB_NAMESPACE::NewBloomFilterPolicy(10);
        options->options.block_cache = ROCKSDB_NAMESPACE::NewLRUCache(40 * 1024 * 1024);
        options->options.write_buffer_size = 4 * 1024 * 1024;
        options->options.info_log = &options->logger;
        options->options.compression = compression_type;
        options->options.block_size = 163840;

        options->read_options.decompress_allocator = &options->decompress_allocator;*/
    }

    RocksDB::RocksDB(RocksDB&& other)
    {
        _impl = other._impl;
        other._impl = nullptr;
    }

    RocksDB& RocksDB::operator=(RocksDB&& other)
    {
        close();
        _impl = other._impl;
        other._impl = nullptr;
        return *this;
    }

    RocksDB::~RocksDB()
    {
        close();
    }

    void RocksDB::close()
    {
        if (_impl) {
            {
                std::lock_guard lock(_impl->iterators_mutex);
                // Create a copy. The delete callback will mutate the original.
                auto iterators = _impl->iterators;
                for (auto& it : iterators) {
                    it->destroy();
                }
                assert(_impl->iterators.empty());
            }

            auto status = _impl->db->Close();
            if (status.IsAborted()) {
                throw std::runtime_error("Failed to close RocksDB: There are unreleased snapshots.");
            }
            delete _impl->db;
            delete _impl;
            _impl = nullptr;
        }
    }

    RocksDB::operator bool() const
    {
        return _impl != nullptr;
    }

    std::unique_ptr<Iterator> RocksDB::create_iterator()
    {
        if (!_impl) {
            throw std::runtime_error("RocksDB has been closed");
        }

        std::lock_guard lock(_impl->iterators_mutex);

        auto* raw_it = _impl->db->NewIterator(_impl->read_options);
        auto* it = new Iterator(raw_it);

        raw_it->RegisterCleanup(remove_iterator, _impl, it);

        _impl->iterators.insert(it);

        return std::unique_ptr<Iterator>(it);
    }

    std::string RocksDB::get(std::string_view key)
    {
        if (!_impl) {
            throw std::runtime_error("RocksDB has been closed");
        }
        std::string value;
        auto status = _impl->db->Get(
            _impl->read_options,
            key,
            &value);
        if (status.ok()) {
            return value;
        } else if (status.IsNotFound()) {
            throw KeyError("key does not exist");
        } else {
            throw std::runtime_error("RocksDB Get error: " + status.ToString());
        }
    }

    bool RocksDB::contains(std::string_view key)
    {
        if (!_impl) {
            throw std::runtime_error("RocksDB has been closed");
        }
        std::string value;
        auto status = _impl->db->Get(
            _impl->read_options,
            key,
            &value);
        if (status.ok()) {
            return true;
        } else if (status.IsNotFound()) {
            return false;
        } else {
            throw std::runtime_error("RocksDB Get error: " + status.ToString());
        }
    }

    void RocksDB::put(std::string_view key, std::string_view value)
    {
        if (!_impl) {
            throw std::runtime_error("RocksDB has been closed");
        }
        auto status = _impl->db->Put(
            _impl->write_options,
            key,
            value);
        if (!status.ok()) {
            throw std::runtime_error("RocksDB Put error: " + status.ToString());
        }
    }

    void RocksDB::put_batch(
        const std::list<std::pair<std::string_view, std::optional<std::string_view>>>& batch)
    {
        ROCKSDB_NAMESPACE::WriteBatch native_batch;
        for (const auto& [key, value] : batch) {
            if (value) {
                native_batch.Put(key, *value);
            } else {
                native_batch.Delete(key);
            }
        }
    }

    void RocksDB::del(std::string_view key)
    {
        if (!_impl) {
            throw std::runtime_error("RocksDB has been closed");
        }
        auto status = _impl->db->Delete(
            _impl->write_options,
            key);
        if (!status.ok()) {
            throw std::runtime_error("RocksDB Delete error: " + status.ToString());
        }
    }

    void RocksDB::compact_range(std::optional<std::string_view> begin, std::optional<std::string_view> end)
    {
        if (!_impl) {
            throw std::runtime_error("RocksDB has been closed");
        }
        if (begin) {
            ROCKSDB_NAMESPACE::Slice begin_slice = *begin;
            if (end) {
                ROCKSDB_NAMESPACE::Slice end_slice = *end;
                _impl->db->CompactRange(
                    _impl->compact_range_options,
                    &begin_slice,
                    &end_slice);
            } else {
                _impl->db->CompactRange(
                    _impl->compact_range_options,
                    &begin_slice,
                    nullptr);
            }
        } else {
            if (end) {
                ROCKSDB_NAMESPACE::Slice end_slice = *end;
                _impl->db->CompactRange(
                    _impl->compact_range_options,
                    nullptr,
                    &end_slice);
            } else {
                _impl->db->CompactRange(
                    _impl->compact_range_options,
                    nullptr,
                    nullptr);
            }
        }
    }

    void RocksDB::compact()
    {
        if (!_impl) {
            throw std::runtime_error("RocksDB has been closed");
        }
        _impl->db->CompactRange(
            _impl->compact_range_options,
            nullptr,
            nullptr);
    }

} // namespace RocksDB
} // namespace Amulet
