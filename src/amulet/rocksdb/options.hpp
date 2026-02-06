#pragma once

#include "compression.hpp"
#include "export.hpp"

namespace ROCKSDB_NAMESPACE {
struct Options;
}

namespace Amulet {
namespace RocksDB {
    class RocksDB;

    class AMULET_ROCKSDB_EXPORT Options {
    private:
        ROCKSDB_NAMESPACE::Options* _impl;

    public:
        Options();
        ~Options();

        Options(const Options&) = delete;
        Options& operator=(const Options&) = delete;
        Options(Options&&);
        Options& operator=(Options&&);

        // Steal the internal pointer
        // It is your responsibiliy to delete the pointer
        // This allocates a new internal pointer
        ROCKSDB_NAMESPACE::Options* steal();

        // Borrow the internal pointer
        // The pointer is still managed by this object
        ROCKSDB_NAMESPACE::Options* borrow();

        bool get_create_if_missing();
        void set_create_if_missing(bool);

        CompressionType get_compression_type();
        void set_compression_type(CompressionType);
    };

} // namespace RocksDB
} // namespace Amulet
