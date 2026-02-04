#include <filesystem>
#include <memory>
#include <string>
#include <string_view>

#include <rocksdb/db.h>

#include "db.hpp"
#include "options.hpp"
#include "read_options.hpp"
#include "write_options.hpp"

namespace Amulet {
namespace RocksDB {

    struct RocksDBImpl {
        ROCKSDB_NAMESPACE::DB* db;
        ROCKSDB_NAMESPACE::ReadOptions read_options;
        ROCKSDB_NAMESPACE::WriteOptions write_options;
        // ROCKSDB_NAMESPACE::CompactRangeOptions compact_range_options;
    };

    RocksDB::RocksDB(
        std::filesystem::path path,
        Options options,
        ReadOptions read_options,
        WriteOptions write_options)
    // CompactRangeOptions& compact_range_options)
    {
        // Expand dots and symbolic links
        path = std::filesystem::absolute(path);

        std::unique_ptr<ROCKSDB_NAMESPACE::DB> db = nullptr;
        auto status = ROCKSDB_NAMESPACE::DB::Open(*options._impl, path.string(), &db);
        if (status.IsCorruption()) {
            ROCKSDB_NAMESPACE::RepairDB(path.string(), *options._impl);
            status = ROCKSDB_NAMESPACE::DB::Open(*options._impl, path.string(), &db);
        }
        if (!status.ok()) {
            throw RocksDBException(status.ToString());
        }
        _impl = new RocksDBImpl {
            db.release(),
            *read_options._impl,
            *write_options._impl,
        };
    }

    RocksDB::RocksDB(
        std::filesystem::path path,
        bool create_if_missing)
        : RocksDB(
              path,
              [create_if_missing]() {
                  Options options;
                  options.set_create_if_missing(create_if_missing);
                  return options;
              }(),
              ReadOptions(),
              WriteOptions())
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
            // Destroy all iterators.
            // TODO

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

    // std::unique_ptr<Iterator> RocksDB::create_iterator()
    //{
    // }

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

    // void RocksDB::compact_range(std::optional<std::string_view> begin, std::optional<std::string_view> end)
    //{
    //     if (!_impl) {
    //         throw std::runtime_error("RocksDB has been closed");
    //     }
    //     _impl->db->CompactRange(
    //         _impl->compact_range_options,
    //         begin ? *begin : nullptr,
    //         end ? *end : nullptr);
    // }

    // void RocksDB::compact()
    //{
    //     if (!_impl) {
    //         throw std::runtime_error("RocksDB has been closed");
    //     }
    //     _impl->db->CompactRange(
    //         _impl->compact_range_options,
    //         nullptr,
    //         nullptr);
    // }

} // namespace RocksDB
} // namespace Amulet
