#pragma once

#include "export.hpp"

namespace ROCKSDB_NAMESPACE {
struct CompactRangeOptions;
}

namespace Amulet {
namespace RocksDB {

    class AMULET_ROCKSDB_EXPORT CompactRangeOptions {
    private:
        ROCKSDB_NAMESPACE::CompactRangeOptions* _impl;

    public:
        CompactRangeOptions();
        ~CompactRangeOptions();

        CompactRangeOptions(const CompactRangeOptions&) = delete;
        CompactRangeOptions& operator=(const CompactRangeOptions&) = delete;
        CompactRangeOptions(CompactRangeOptions&&);
        CompactRangeOptions& operator=(CompactRangeOptions&&);

        // Steal the internal pointer
        // It is your responsibiliy to delete the pointer
        // This allocates a new internal pointer
        ROCKSDB_NAMESPACE::CompactRangeOptions* steal();

        // Borrow the internal pointer
        // The pointer is still managed by this object
        ROCKSDB_NAMESPACE::CompactRangeOptions* borrow();
    };

} // namespace RocksDB
} // namespace Amulet
