#include <rocksdb/options.h>

#include "read_options.hpp"

namespace Amulet {
namespace RocksDB {

    ReadOptions::ReadOptions()
        : _impl(new ROCKSDB_NAMESPACE::ReadOptions())
    {
    }

    ReadOptions::~ReadOptions()
    {
        delete _impl;
    }

    ReadOptions::ReadOptions(ReadOptions&& other)
    {
        _impl = other._impl;
        other._impl = new ROCKSDB_NAMESPACE::ReadOptions();
    }

    ReadOptions& ReadOptions::operator=(ReadOptions&& other)
    {
        delete _impl;
        _impl = other._impl;
        other._impl = new ROCKSDB_NAMESPACE::ReadOptions();
        return *this;
    }

} // namespace RocksDB
} // namespace Amulet
