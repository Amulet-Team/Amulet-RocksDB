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
        _impl = other.steal();
    }

    ReadOptions& ReadOptions::operator=(ReadOptions&& other)
    {
        delete _impl;
        _impl = other.steal();
        return *this;
    }

    ROCKSDB_NAMESPACE::ReadOptions* ReadOptions::steal()
    {
        auto* impl = _impl;
        _impl = new ROCKSDB_NAMESPACE::ReadOptions();
        return impl;
    }

    ROCKSDB_NAMESPACE::ReadOptions* ReadOptions::borrow()
    {
        return _impl;
    }

} // namespace RocksDB
} // namespace Amulet
