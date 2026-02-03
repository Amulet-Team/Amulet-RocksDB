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

    Options::Options(Options&& other) {
        _impl = other._impl;
        other._impl = new ROCKSDB_NAMESPACE::Options();
    }

    Options& Options::operator=(Options&& other) {
        delete _impl;
        _impl = other._impl;
        other._impl = new ROCKSDB_NAMESPACE::Options();
        return *this;
    }

    bool Options::create_if_misssing()
    {
        return _impl->create_if_missing;
    }

    void Options::set_create_if_missing(bool create_if_missing)
    {
        _impl->create_if_missing = create_if_missing;
    }

} // namespace RocksDB
} // namespace Amulet
