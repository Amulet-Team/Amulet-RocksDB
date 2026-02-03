#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>

#include "export.hpp"

namespace ROCKSDB_NAMESPACE {
class DB;
}

namespace Amulet {
namespace RocksDB {
    class Options;
    class ReadOptions;
    class WriteOptions;
    class CompactRangeOptions;
    class Iterator;

    class AMULET_ROCKSDB_EXPORT_EXCEPTION RocksDBException : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    class AMULET_ROCKSDB_EXPORT_EXCEPTION KeyError : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
        KeyError()
            : KeyError("KeyError")
        {
        }
    };

    struct RocksDBImpl;

    // A wrapper for RocksDB.
    // Handles cleaning up iterators when the database is closed.
    class AMULET_ROCKSDB_EXPORT RocksDB {
    private:
        RocksDBImpl* _impl;

    public:
        // Constructor
        RocksDB(
            std::filesystem::path path,
            Options options,
            ReadOptions read_options,
            WriteOptions write_options);
        // CompactRangeOptions& compact_range_options);

        RocksDB(
            std::filesystem::path path,
            bool create_if_missing = false
        );

        // Copy (deleted)
        RocksDB(const RocksDB&) = delete;
        RocksDB& operator=(const RocksDB&) = delete;

        // Move
        // Not thread safe.
        RocksDB(RocksDB&&);
        RocksDB& operator=(RocksDB&&);

        // Destructor
        // Closes the database if it is not already closed.
        // Not thread safe.
        ~RocksDB();

        // Close the database and delete all iterators.
        // This must only be called by the owner.
        // Not thread safe
        void close();

        // Is the database valid?
        // If this returns false, all other calls will fail.
        operator bool() const;

        // All methods below are thread safe with each other.
        // They must not be called in parallel with any non-const method above.

        // Create an iterator that is automatically destroyed when the database is closed.
        // You may use raw iterators but you must ensure the database outlives the iterator.
        // std::unique_ptr<Iterator> create_iterator();

        // Get the value for a given key.
        // Thread safe.
        std::string get(std::string_view key);

        // Set a value for the given key.
        // Thread safe.
        void put(std::string_view key, std::string_view value);

        // Delete the given key.
        // Thread safe.
        void del(std::string_view key);

        //// Compact the range of the database
        //// If an argument is nullopt it defaults to the start or end of the database.
        //// Thread safe.
        // void compact_range(
        //     std::optional<std::string_view> begin,
        //     std::optional<std::string_view> end);

        //// Compact the whole database
        //// Thread safe.
        // void compact();
    };
} // namespace RocksDB
} // namespace Amulet
