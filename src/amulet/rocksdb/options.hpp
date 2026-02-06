#pragma once

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
        friend RocksDB;

    public:
        Options();
        ~Options();

        Options(const Options&) = delete;
        Options& operator=(const Options&) = delete;
        Options(Options&&);
        Options& operator=(Options&&);

        bool get_create_if_missing();
        void set_create_if_missing(bool);
    };

} // namespace RocksDB
} // namespace Amulet
