#include <rocksdb/options.h>

#include "options.hpp"

namespace Amulet {
namespace RocksDB {

    Options::Options()
        : _impl(new ROCKSDB_NAMESPACE::Options())
    {
    }

    Options::~Options()
    {
        delete _impl;
    }

    Options::Options(Options&& other)
    {
        _impl = other.steal();
    }

    Options& Options::operator=(Options&& other)
    {
        delete _impl;
        _impl = other.steal();
        return *this;
    }

    ROCKSDB_NAMESPACE::Options* Options::steal()
    {
        auto* impl = _impl;
        _impl = new ROCKSDB_NAMESPACE::Options();
        return impl;
    }

    ROCKSDB_NAMESPACE::Options* Options::borrow()
    {
        return _impl;
    }

    bool Options::get_create_if_missing()
    {
        return _impl->create_if_missing;
    }

    void Options::set_create_if_missing(bool create_if_missing)
    {
        _impl->create_if_missing = create_if_missing;
    }

    CompressionType Options::get_compression_type()
    {
        return static_cast<CompressionType>(_impl->compression);
    }

    void Options::set_compression_type(CompressionType compression_type)
    {
        _impl->compression = static_cast<ROCKSDB_NAMESPACE::CompressionType>(compression_type);
    }

} // namespace RocksDB
} // namespace Amulet
