#include <rocksdb/options.h>

#include "compact_range_options.hpp"

namespace Amulet {
namespace RocksDB {

    CompactRangeOptions::CompactRangeOptions()
        : _impl(new ROCKSDB_NAMESPACE::CompactRangeOptions())
    {
    }

    CompactRangeOptions::~CompactRangeOptions()
    {
        delete _impl;
    }

    CompactRangeOptions::CompactRangeOptions(CompactRangeOptions&& other)
    {
        _impl = other.steal();
    }

    CompactRangeOptions& CompactRangeOptions::operator=(CompactRangeOptions&& other)
    {
        delete _impl;
        _impl = other.steal();
        return *this;
    }

    ROCKSDB_NAMESPACE::CompactRangeOptions* CompactRangeOptions::steal()
    {
        auto* impl = _impl;
        _impl = new ROCKSDB_NAMESPACE::CompactRangeOptions();
        return impl;
    }

    ROCKSDB_NAMESPACE::CompactRangeOptions* CompactRangeOptions::borrow()
    {
        return _impl;
    }

} // namespace RocksDB
} // namespace Amulet
