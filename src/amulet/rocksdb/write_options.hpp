#pragma once

#include "export.hpp"

namespace ROCKSDB_NAMESPACE {
struct WriteOptions;
}

namespace Amulet {
namespace RocksDB {
    class RocksDB;

    class AMULET_ROCKSDB_EXPORT WriteOptions {
    private:
        ROCKSDB_NAMESPACE::WriteOptions* _impl;

    public:
        WriteOptions();
        ~WriteOptions();

        WriteOptions(const WriteOptions&) = delete;
        WriteOptions& operator=(const WriteOptions&) = delete;
        WriteOptions(WriteOptions&&);
        WriteOptions& operator=(WriteOptions&&);

        // Steal the internal pointer
        // It is your responsibiliy to delete the pointer
        // This allocates a new internal pointer
        ROCKSDB_NAMESPACE::WriteOptions* steal();

        // Borrow the internal pointer
        // The pointer is still managed by this object
        ROCKSDB_NAMESPACE::WriteOptions* borrow();

        bool get_sync();
        void set_sync(bool);

        bool get_disable_wal();
        void set_disable_wal(bool);
    };

} // namespace RocksDB
} // namespace Amulet
