#pragma once

#include "export.hpp"

namespace ROCKSDB_NAMESPACE {
struct ReadOptions;
}

namespace Amulet {
namespace RocksDB {
    class RocksDB;

    class AMULET_ROCKSDB_EXPORT ReadOptions {
    private:
        ROCKSDB_NAMESPACE::ReadOptions* _impl;

    public:
        ReadOptions();
        ~ReadOptions();

        ReadOptions(const ReadOptions&) = delete;
        ReadOptions& operator=(const ReadOptions&) = delete;
        ReadOptions(ReadOptions&&);
        ReadOptions& operator=(ReadOptions&&);

        // Steal the internal pointer
        // It is your responsibiliy to delete the pointer
        // This allocates a new internal pointer
        ROCKSDB_NAMESPACE::ReadOptions* steal();

        // Borrow the internal pointer
        // The pointer is still managed by this object
        ROCKSDB_NAMESPACE::ReadOptions* borrow();
    };

} // namespace RocksDB
} // namespace Amulet
