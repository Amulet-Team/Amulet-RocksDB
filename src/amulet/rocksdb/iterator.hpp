#pragma once

#include "export.hpp"

namespace ROCKSDB_NAMESPACE {
class Iterator;
}

namespace Amulet {
namespace RocksDB {
    class RocksDB;
    class IteratorImpl;

    class AMULET_ROCKSDB_EXPORT Iterator {
    private:
        IteratorImpl* _impl;

        // Destroy the internal pointer.
        // This object will still exist but all methods will error if called.
        void destroy();

        friend RocksDB;

    public:
        Iterator(ROCKSDB_NAMESPACE::Iterator*);
        ~Iterator();

        Iterator(const Iterator&) = delete;
        Iterator& operator=(const Iterator&) = delete;
        Iterator(Iterator&&) = delete;
        Iterator& operator=(Iterator&&) = delete;

        // Is the internal pointer valid.
        // If false other calls with throw runtime_error
        operator bool() const;

        // Is the iterator valid.
        // If false other calls with throw runtime_error
        bool is_valid() const;

        // Get the key of the current entry in the database.
        // Throws runtime_error if iterator is not valid.
        std::string_view key() const;

        // Get the value of the current entry in the database.
        // Throws runtime_error if iterator is not valid.
        std::string_view value() const;

        // Seek to the first entry in the database.
        void seek_to_first();

        // Seek to the last entry in the database.
        void seek_to_last();

        // Seek to the given entry in the database.
        // If the entry does not exist it will seek to the location after.
        void seek(std::string_view key);

        // Seek to the next entry in the database.
        void next();

        // Seek to the previous entry in the database.
        void prev();
    };

} // namespace RocksDB
} // namespace Amulet
