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
        friend RocksDB;

    public:
        WriteOptions();
        ~WriteOptions();

        WriteOptions(const WriteOptions&) = delete;
        WriteOptions& operator=(const WriteOptions&) = delete;
        WriteOptions(WriteOptions&&);
        WriteOptions& operator=(WriteOptions&&);

        bool sync();
        void set_sync(bool);

        bool disable_wal();
        void set_disable_wal(bool);
    };

} // namespace RocksDB
} // namespace Amulet
