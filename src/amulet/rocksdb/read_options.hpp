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
        friend RocksDB;

    public:
        ReadOptions();
        ~ReadOptions();

        ReadOptions(const ReadOptions&) = delete;
        ReadOptions& operator=(const ReadOptions&) = delete;
        ReadOptions(ReadOptions&&);
        ReadOptions& operator=(ReadOptions&&);
    };

} // namespace RocksDB
} // namespace Amulet
