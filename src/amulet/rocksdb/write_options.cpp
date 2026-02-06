#include <rocksdb/options.h>

#include "write_options.hpp"

namespace Amulet {
namespace RocksDB {

    WriteOptions::WriteOptions()
        : _impl(new ROCKSDB_NAMESPACE::WriteOptions())
    {
    }

    WriteOptions::~WriteOptions()
    {
        delete _impl;
    }

    WriteOptions::WriteOptions(WriteOptions&& other)
    {
        _impl = other._impl;
        other._impl = new ROCKSDB_NAMESPACE::WriteOptions();
    }

    WriteOptions& WriteOptions::operator=(WriteOptions&& other)
    {
        delete _impl;
        _impl = other._impl;
        other._impl = new ROCKSDB_NAMESPACE::WriteOptions();
        return *this;
    }

    bool WriteOptions::get_sync()
    {
        return _impl->sync;
    }

    void WriteOptions::set_sync(bool sync)
    {
        _impl->sync = sync;
    }

    bool WriteOptions::get_disable_wal()
    {
        return _impl->disableWAL;
    }

    void WriteOptions::set_disable_wal(bool disable_wal)
    {
        _impl->disableWAL = disable_wal;
    }

} // namespace RocksDB
} // namespace Amulet
