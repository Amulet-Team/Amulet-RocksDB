#include <shared_mutex>

#include <rocksdb/iterator.h>

#include "iterator.hpp"

namespace Amulet {
namespace RocksDB {

    class IteratorImpl {
    public:
        ROCKSDB_NAMESPACE::Iterator* it;
        std::shared_mutex mutex;

        IteratorImpl(ROCKSDB_NAMESPACE::Iterator* it)
            : it(it)
        {
        }
    };

    Iterator::Iterator(ROCKSDB_NAMESPACE::Iterator* it)
        : _impl(new IteratorImpl(it))
    {
    }

    Iterator::~Iterator()
    {
        destroy();
        delete _impl;
    }

    void Iterator::destroy()
    {
        std::lock_guard lock(_impl->mutex);
        if (_impl->it) {
            delete _impl->it;
            _impl->it = nullptr;
        }
    }

    Iterator::operator bool() const
    {
        std::shared_lock lock(_impl->mutex);
        return _impl->it != nullptr;
    }

    bool Iterator::is_valid() const
    {
        std::shared_lock lock(_impl->mutex);
        if (_impl->it == nullptr) {
            throw std::runtime_error("The iterator has been destroyed");
        }
        return _impl->it->Valid();
    }

    std::string_view Iterator::key() const
    {
        std::shared_lock lock(_impl->mutex);
        if (_impl->it == nullptr) {
            throw std::runtime_error("The iterator has been destroyed");
        }
        auto key = _impl->it->key();
        return { key.data(), key.size() };
    }

    std::string_view Iterator::value() const
    {
        std::shared_lock lock(_impl->mutex);
        if (_impl->it == nullptr) {
            throw std::runtime_error("The iterator has been destroyed");
        }
        auto value = _impl->it->value();
        return { value.data(), value.size() };
    }

    void Iterator::seek_to_first()
    {
        std::shared_lock lock(_impl->mutex);
        if (_impl->it == nullptr) {
            throw std::runtime_error("The iterator has been destroyed");
        }
        _impl->it->SeekToFirst();
    }

    void Iterator::seek_to_last()
    {
        std::shared_lock lock(_impl->mutex);
        if (_impl->it == nullptr) {
            throw std::runtime_error("The iterator has been destroyed");
        }
        _impl->it->SeekToLast();
    }

    void Iterator::seek(std::string_view key)
    {
        std::shared_lock lock(_impl->mutex);
        if (_impl->it == nullptr) {
            throw std::runtime_error("The iterator has been destroyed");
        }
        _impl->it->Seek(key);
    }

    void Iterator::next()
    {
        std::shared_lock lock(_impl->mutex);
        if (_impl->it == nullptr) {
            throw std::runtime_error("The iterator has been destroyed");
        }
        _impl->it->Next();
    }

    void Iterator::prev()
    {
        std::shared_lock lock(_impl->mutex);
        if (_impl->it == nullptr) {
            throw std::runtime_error("The iterator has been destroyed");
        }
        _impl->it->Prev();
    }

} // namespace RocksDB
} // namespace Amulet
