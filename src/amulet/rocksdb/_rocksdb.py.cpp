#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>
#include <pybind11/typing.h>

// #include <filesystem>
// #include <optional>
// #include <string>
#include <string_view>
// #include <variant>
//
// #include <rocksdb/cache.h>
// #include <rocksdb/db.h>
////#include <rocksdb/decompress_allocator.h>
// #include <rocksdb/env.h>
// #include <rocksdb/filter_policy.h>
// #include <rocksdb/options.h>
// #include <rocksdb/write_batch.h>

#include <amulet/pybind11_extensions/compatibility.hpp>
#include <amulet/pybind11_extensions/iterator.hpp>

// #include <amulet/rocksdb.hpp>
#include <amulet/rocksdb/db.hpp>

namespace py = pybind11;
namespace pyext = Amulet::pybind11_extensions;

namespace PYBIND11_NAMESPACE {
namespace detail {
    // template <>
    // struct type_caster<rocksdb::Slice> {
    // public:
    //     PYBIND11_TYPE_CASTER(rocksdb::Slice, const_name("bytes"));

    //    bool load(handle src, bool)
    //    {
    //        PyObject* source = src.ptr();
    //        if (!PyBytes_Check(source)) {
    //            return false;
    //        }
    //        Py_ssize_t size = PyBytes_Size(src.ptr());
    //        const char* buffer = PyBytes_AsString(src.ptr());
    //        if (!buffer) {
    //            return false;
    //        }
    //        value = rocksdb::Slice(buffer, size);
    //        return true;
    //    }

    //    // This causes a crash that I don't understand
    //    // static handle cast(const rocksdb::Slice& src, return_value_policy /* policy */, handle /* parent */)
    //    //{
    //    //    return py::bytes(src.data(), src.size());
    //    //}
    //};

    // template <>
    // struct type_caster<rocksdb::WriteBatch> {
    // public:
    //     PYBIND11_TYPE_CASTER(rocksdb::WriteBatch, const_name("collections.abc.Mapping[bytes, bytes]"));

    //    bool load(handle src, bool)
    //    {
    //        auto getitem = src.attr("__getitem__");
    //        for (auto& key : src) {
    //            if (!PyBytes_Check(key.ptr())) {
    //                return false;
    //            }
    //            Py_ssize_t key_size = PyBytes_Size(key.ptr());
    //            const char* key_buffer = PyBytes_AsString(key.ptr());
    //            if (!key_buffer) {
    //                return false;
    //            }

    //            auto val = getitem(key);
    //            if (val.is_none()) {
    //                value.Delete(rocksdb::Slice(key_buffer, key_size));
    //            } else {
    //                Py_ssize_t val_size = PyBytes_Size(val.ptr());
    //                const char* val_buffer = PyBytes_AsString(val.ptr());
    //                if (!val_buffer) {
    //                    return false;
    //                }
    //                value.Put(rocksdb::Slice(key_buffer, key_size), rocksdb::Slice(val_buffer, val_size));
    //            }
    //        }
    //        return true;
    //    }
    //};
}
} // namespace PYBIND11_NAMESPACE::detail

namespace {

// class RocksDBException : public std::runtime_error {
//     using std::runtime_error::runtime_error;
// };
// class RocksDBEncrypted : public RocksDBException {
//     using RocksDBException::RocksDBException;
// };

// class NullLogger : public rocksdb::Logger {
// public:
//     void Logv(const char*, va_list) override { }
// };
//
// class RocksDBOptions : public Amulet::RocksDBOptions {
// public:
//     NullLogger logger;
//     //rocksdb::DecompressAllocator decompress_allocator;
// };
//
// std::unique_ptr<Amulet::RocksDB> open_rocksdb(
//     std::string path_str,
//     bool create_if_missing = false,
//     rocksdb::CompressionType compression_type = rocksdb::kZlibCompression)
//{
//     // Expand dots and symbolic links
//     auto path = std::filesystem::absolute(path_str);
//     // If there is not a directory at the path
//     if (!std::filesystem::is_directory(path)) {
//         if (std::filesystem::exists(path)) {
//             // If the path exists but is not a directory
//             throw RocksDBException("A non-directory file exists at " + path.string());
//         } else if (create_if_missing) {
//             // Create if requested
//             std::filesystem::create_directories(path);
//         } else {
//             throw RocksDBException("No database exists to open at " + path.string());
//         }
//     }
//
//     auto options = std::make_unique<RocksDBOptions>();
//     options->options.create_if_missing = create_if_missing;
//     //options->options.filter_policy = rocksdb::NewBloomFilterPolicy(10);
//     //options->options.block_cache = rocksdb::NewLRUCache(40 * 1024 * 1024);
//     options->options.write_buffer_size = 4 * 1024 * 1024;
//     //options->options.info_log = &options->logger;
//     options->options.compression = compression_type;
//     //options->options.block_size = 163840;
//
//     //options->read_options.decompress_allocator = &options->decompress_allocator;
//
//     rocksdb::DB* _db = NULL;
//     auto status = rocksdb::DB::Open(options->options, path.string(), &_db);
//     if (status.ok()) {
//         return std::make_unique<Amulet::RocksDB>(
//             std::unique_ptr<rocksdb::DB>(_db),
//             std::move(options));
//     } else if (status.IsCorruption()) {
//         rocksdb::RepairDB(path.string(), options->options);
//         {
//             auto status2 = rocksdb::DB::Open(options->options, path.string(), &_db);
//             if (status2.ok()) {
//                 return std::make_unique<Amulet::RocksDB>(
//                     std::unique_ptr<rocksdb::DB>(_db),
//                     std::move(options));
//             }
//         }
//         throw RocksDBException("Could not recover corrupted database. " + status.ToString());
//     }
//     throw RocksDBException(status.ToString());
// }
//
// class RocksDBKeysIterator {
// private:
//     std::unique_ptr<Amulet::RocksDBIterator> iterator_ptr;
//
// public:
//     RocksDBKeysIterator(
//         std::unique_ptr<Amulet::RocksDBIterator> iterator_ptr)
//         : iterator_ptr(std::move(iterator_ptr))
//     {
//     }
//
//     py::bytes next()
//     {
//         auto& iterator = *iterator_ptr;
//         if (!iterator) {
//             throw std::runtime_error("RocksDBIterator has been deleted.");
//         }
//         if (!iterator->Valid()) {
//             throw py::stop_iteration();
//         }
//         // Get value.
//         auto key = py::bytes(iterator->key().ToString());
//         // Increment for next time.
//         iterator->Next();
//         // Return value
//         return key;
//     }
// };
//
// class RocksDBValuesIterator {
// private:
//     std::unique_ptr<Amulet::RocksDBIterator> iterator_ptr;
//
// public:
//     RocksDBValuesIterator(
//         std::unique_ptr<Amulet::RocksDBIterator> iterator_ptr)
//         : iterator_ptr(std::move(iterator_ptr))
//     {
//     }
//
//     py::bytes next()
//     {
//         auto& iterator = *iterator_ptr;
//         if (!iterator) {
//             throw std::runtime_error("RocksDBIterator has been deleted.");
//         }
//         if (!iterator->Valid()) {
//             throw py::stop_iteration();
//         }
//         // Get value.
//         auto value = py::bytes(iterator->value().ToString());
//         // Increment for next time.
//         iterator->Next();
//         // Return value
//         return value;
//     }
// };
//
// class RocksDBItemsIterator {
// private:
//     std::unique_ptr<Amulet::RocksDBIterator> iterator_ptr;
//
// public:
//     RocksDBItemsIterator(
//         std::unique_ptr<Amulet::RocksDBIterator> iterator_ptr)
//         : iterator_ptr(std::move(iterator_ptr))
//     {
//     }
//
//     py::typing::Tuple<py::bytes, py::bytes> next()
//     {
//         auto& iterator = *iterator_ptr;
//         if (!iterator) {
//             throw std::runtime_error("RocksDBIterator has been deleted.");
//         }
//         if (!iterator->Valid()) {
//             throw py::stop_iteration();
//         }
//         // Get value.
//         auto item = py::make_tuple(
//             py::bytes(iterator->key().ToString()),
//             py::bytes(iterator->value().ToString()));
//         // Increment for next time.
//         iterator->Next();
//         // Return value
//         return item;
//     }
// };
//
// class RocksDBItemsRangeIterator {
// private:
//     std::unique_ptr<Amulet::RocksDBIterator> iterator_ptr;
//     std::optional<std::string> end;
//
// public:
//     RocksDBItemsRangeIterator(
//         std::unique_ptr<Amulet::RocksDBIterator> iterator_ptr,
//         std::string end)
//         : iterator_ptr(std::move(iterator_ptr))
//         , end(end)
//     {
//     }
//
//     py::typing::Tuple<py::bytes, py::bytes> next()
//     {
//         auto& iterator = *iterator_ptr;
//         if (!iterator) {
//             throw std::runtime_error("RocksDBIterator has been deleted.");
//         }
//         if (!iterator->Valid()) {
//             throw py::stop_iteration();
//         }
//         // Get value.
//         std::string key = iterator->key().ToString();
//         if (end <= key) {
//             throw py::stop_iteration();
//         }
//         auto item = py::make_tuple(
//             py::bytes(key),
//             py::bytes(iterator->value().ToString()));
//         // Increment for next time.
//         iterator->Next();
//         // Return value
//         return item;
//     }
// };
//
// static std::unique_ptr<Amulet::RocksDBIterator> get_start_iterator(Amulet::RocksDB& db)
//{
//     py::gil_scoped_release nogil;
//     if (!db) {
//         throw std::runtime_error("The RocksDB database has been closed.");
//     }
//     auto iterator_ptr = db.create_iterator();
//     auto& iterator = *iterator_ptr;
//     iterator->SeekToFirst();
//     return iterator_ptr;
// }

} // namespace

void init_module(py::module m)
{
    pyext::init_compiler_config(m);
    std::string module_name = m.attr("__name__").cast<std::string>();

    py::register_local_exception<Amulet::RocksDB::RocksDBException>(m, "RocksDBException");
    // py::register_local_exception<RocksDBEncrypted>(m, "RocksDBEncrypted");

    // py::classh<Amulet::RocksDBIterator> RocksDBIterator(m, "RocksDBIterator", py::release_gil_before_calling_cpp_dtor());
    // RocksDBIterator.def(
    //     "valid",
    //     [](Amulet::RocksDBIterator& self) {
    //         return self && self->Valid();
    //     },
    //     py::doc(
    //         "Is the iterator at a valid entry.\n"
    //         "If False, calls to other methods may error."));
    // RocksDBIterator.def(
    //     "seek_to_first",
    //     [](Amulet::RocksDBIterator& self) {
    //         if (!self) {
    //             throw std::runtime_error("RocksDBIterator has been deleted.");
    //         }
    //         self->SeekToFirst();
    //     },
    //     py::doc("Seek to the first entry in the database."));
    // RocksDBIterator.def(
    //     "seek_to_last",
    //     [](Amulet::RocksDBIterator& self) {
    //         if (!self) {
    //             throw std::runtime_error("RocksDBIterator has been deleted.");
    //         }
    //         self->SeekToLast();
    //     },
    //     py::doc("Seek to the last entry in the database."));
    // RocksDBIterator.def(
    //     "seek",
    //     [](Amulet::RocksDBIterator& self, rocksdb::Slice target) {
    //         if (!self) {
    //             throw std::runtime_error("RocksDBIterator has been deleted.");
    //         }
    //         self->Seek(target);
    //     },
    //     py::arg("target"),
    //     py::doc(
    //         "Seek to the given entry in the database.\n"
    //         "If the entry does not exist it will seek to the location after."));
    // RocksDBIterator.def(
    //     "next",
    //     [](Amulet::RocksDBIterator& self) {
    //         if (!self) {
    //             throw std::runtime_error("RocksDBIterator has been deleted.");
    //         }
    //         self->Next();
    //     },
    //     py::doc(
    //         "Seek to the next entry in the database."));
    // RocksDBIterator.def(
    //     "prev",
    //     [](Amulet::RocksDBIterator& self) {
    //         if (!self) {
    //             throw std::runtime_error("RocksDBIterator has been deleted.");
    //         }
    //         self->Prev();
    //     },
    //     py::doc(
    //         "Seek to the previous entry in the database."));
    // RocksDBIterator.def(
    //     "key",
    //     [](Amulet::RocksDBIterator& self) {
    //         if (!self) {
    //             throw std::runtime_error("RocksDBIterator has been deleted.");
    //         }
    //         if (!self->Valid()) {
    //             throw std::runtime_error("RocksDBIterator does not point to a valid value.");
    //         }
    //         return py::bytes(self->key().data(), self->key().size());
    //     },
    //     py::doc(
    //         "Get the key of the current entry in the database.\n"
    //         ":raises: runtime_error if iterator is not valid."));
    // RocksDBIterator.def(
    //     "value",
    //     [](Amulet::RocksDBIterator& self) {
    //         if (!self) {
    //             throw std::runtime_error("RocksDBIterator has been deleted.");
    //         }
    //         if (!self->Valid()) {
    //             throw std::runtime_error("RocksDBIterator does not point to a valid value.");
    //         }
    //         return py::bytes(self->value().data(), self->value().size());
    //     },
    //     py::doc(
    //         "Get the value of the current entry in the database.\n"
    //         ":raises: runtime_error if iterator is not valid."));

    py::enum_<Amulet::RocksDB::CompressionType> CompressionType(m, "CompressionType");
    CompressionType.value(
        "NoCompression",
        Amulet::RocksDB::CompressionType::NoCompression,
        "No compression.");
    CompressionType.value(
        "ZstdCompression",
        Amulet::RocksDB::CompressionType::ZStandardCompression,
        "Zstd compression.");
     CompressionType.attr("__repr__") = py::cpp_function(
         [module_name, CompressionType](const py::object& arg) -> py::str {
             return py::str("{}.{}").format(module_name, CompressionType.attr("__str__")(arg));
         },
         py::name("__repr__"),
         py::is_method(CompressionType));

    py::classh<Amulet::RocksDB::RocksDB> RocksDB(m, "RocksDB", py::release_gil_before_calling_cpp_dtor(),
        "A RocksDB database");
    RocksDB.def(
        py::init<std::filesystem::path, bool, Amulet::RocksDB::CompressionType>(),
        py::arg("path"),
        py::arg("create_if_missing") = false,
        py::arg("compression_type") = Amulet::RocksDB::CompressionType::ZStandardCompression,
        py::doc(
            "Construct a new :class:`RocksDB` instance from the database at the given path.\n"
            "\n"
            "A rocksdb database is like a dictionary that only contains bytes as the keys and values and exists entirely on the disk.\n"
            "\n"
            ":param path: The path to the database directory.\n"
            ":param create_if_missing: If True a new database will be created if one does not exist at the given path.\n"
            ":param compression_type: The compression type to use. (Default ZStandardCompression)\n"
            ":raises: RocksDBException if an error occured."));

    RocksDB.def(
        "close",
        &Amulet::RocksDB::RocksDB::close,
        py::doc(
            "Close the rocksdb database.\n"
            "Only the owner of the database may close it.\n"
            "If needed, an external lock must be used to ensure that no other threads are accessing the database."),
        py::call_guard<py::gil_scoped_release>());

    // RocksDB.def(
    //     "compact",
    //     [](Amulet::RocksDB& self) {
    //         if (!self) {
    //             throw std::runtime_error("The RocksDB database has been closed.");
    //         }
    //         //self->CompactRange(nullptr, nullptr);
    //     },
    //     py::doc("Remove deleted entries from the database to reduce its size."),
    //     py::call_guard<py::gil_scoped_release>());

    auto put = [](Amulet::RocksDB::RocksDB& self, py::bytes key, py::bytes value) {
        std::string_view key_view = key;
        std::string_view value_view = value;
        {
            py::gil_scoped_release gil;
            self.put(key_view, value_view);
        }
    };
    RocksDB.def(
        "put",
        put,
        py::arg("key"),
        py::arg("value"),
        py::doc("Set a value in the database."));
    RocksDB.def(
        "__setitem__",
        put,
        py::arg("key"),
        py::arg("value"));

    // RocksDB.def(
    //     "put_batch",
    //     [](Amulet::RocksDB& self, rocksdb::WriteBatch batch) {
    //         if (!self) {
    //             throw std::runtime_error("The RocksDB database has been closed.");
    //         }
    //         rocksdb::Status status = self->Write(self.get_write_options(), &batch);
    //         if (!status.ok()) {
    //             throw RocksDBException(status.ToString());
    //         }
    //     },
    //     py::arg("batch"),
    //     py::doc("Set a group of values in the database."),
    //     py::call_guard<py::gil_scoped_release>());

    // RocksDB.def(
    //     "__contains__",
    //     [](Amulet::RocksDB& self, rocksdb::Slice key) {
    //         if (!self) {
    //             throw std::runtime_error("The RocksDB database has been closed.");
    //         }
    //         std::string value;
    //         return self->Get(self.get_read_options(), key, &value).ok();
    //     },
    //     py::arg("key"),
    //     py::call_guard<py::gil_scoped_release>());

    auto get = [](Amulet::RocksDB::RocksDB& self, py::bytes key) {
        std::string_view key_view = key;
        std::string value;
        try {
            py::gil_scoped_release gil;
            value = self.get(key_view);
        } catch (const Amulet::RocksDB::KeyError& e) {
            throw py::key_error(e.what());
        }
        return py::bytes(value);
    };
    RocksDB.def(
        "get",
        get,
        py::arg("key"),
        py::doc(
            "Get a key from the database.\n"
            "\n"
            ":param key: The key to get from the database.\n"
            ":return: The data stored behind the given key.\n"
            ":raises: KeyError if the requested key is not present.\n"
            ":raises: RocksDBException on other error."));
    RocksDB.def("__getitem__", get, py::arg("key"));

    auto del = [](Amulet::RocksDB::RocksDB& self, py::bytes key) {
        std::string_view key_view = key;
        {
            py::gil_scoped_release gil;
            self.del(key_view);
        }
    };
    RocksDB.def(
        "delete",
        del,
        py::arg("key"),
        py::doc(
            "Delete a key from the database.\n"
            "\n"
            ":param key: The key to delete from the database."),
        py::call_guard<py::gil_scoped_release>());
    RocksDB.def(
        "__delitem__",
        del,
        py::arg("key"),
        py::call_guard<py::gil_scoped_release>());

    // RocksDB.def(
    //     "create_iterator",
    //     &Amulet::RocksDB::create_iterator,
    //     py::doc("Create a new rocksdb Iterator."),
    //     py::call_guard<py::gil_scoped_release>());

    // RocksDB.def(
    //     "iterate",
    //     [](
    //         Amulet::RocksDB& self,
    //         std::optional<py::bytes> start,
    //         std::optional<py::bytes> end) {
    //         if (!self) {
    //             throw std::runtime_error("The RocksDB database has been closed.");
    //         }
    //         std::unique_ptr<Amulet::RocksDBIterator> iterator_ptr;
    //         {
    //             py::gil_scoped_release nogil;
    //             iterator_ptr = self.create_iterator();
    //         }
    //         auto& iterator = *iterator_ptr;
    //         if (start) {
    //             iterator->Seek(start->cast<std::string>());
    //         } else {
    //             iterator->SeekToFirst();
    //         }

    //        if (end) {
    //            return pyext::make_iterator(
    //                RocksDBItemsRangeIterator(std::move(iterator_ptr), end->cast<std::string>()));
    //        } else {
    //            return pyext::make_iterator(
    //                RocksDBItemsIterator(std::move(iterator_ptr)));
    //        }
    //    },
    //    py::arg("start") = py::none(),
    //    py::arg("end") = py::none(),
    //    py::doc(
    //        "Iterate through all keys and data that exist between the given keys.\n"
    //        "\n"
    //        ":param start: The key to start at. Leave as None to start at the beginning.\n"
    //        ":param end: The key to end at. Leave as None to finish at the end."));

    // RocksDB.def(
    //     "__iter__",
    //     [](Amulet::RocksDB& self) {
    //         return pyext::make_iterator(
    //             RocksDBKeysIterator(get_start_iterator(self)));
    //     });
    // RocksDB.def(
    //     "keys",
    //     [](Amulet::RocksDB& self) {
    //         return pyext::make_iterator(
    //             RocksDBKeysIterator(get_start_iterator(self)));
    //     },
    //     py::doc("An iterable of all keys in the database."));

    // RocksDB.def(
    //     "values",
    //     [](Amulet::RocksDB& self) {
    //         return pyext::make_iterator(
    //             RocksDBValuesIterator(get_start_iterator(self)));
    //     },
    //     py::doc("An iterable of all values in the database."));

    // RocksDB.def(
    //     "items",
    //     [](Amulet::RocksDB& self) {
    //         return pyext::make_iterator(
    //             RocksDBItemsIterator(get_start_iterator(self)));
    //     },
    //     py::doc("An iterable of all items in the database."));
}

PYBIND11_MODULE(_rocksdb, m)
{
    m.def("init", &init_module, py::arg("m"));
}
