#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>
#include <pybind11/typing.h>

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

#include <amulet/pybind11_extensions/compatibility.hpp>
#include <amulet/pybind11_extensions/iterable.hpp>
#include <amulet/pybind11_extensions/iterator.hpp>

#include <amulet/rocksdb/compact_range_options.hpp>
#include <amulet/rocksdb/db.hpp>
#include <amulet/rocksdb/options.hpp>
#include <amulet/rocksdb/read_options.hpp>
#include <amulet/rocksdb/write_options.hpp>

namespace py = pybind11;
namespace pyext = Amulet::pybind11_extensions;

namespace {

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

    py::classh<Amulet::RocksDB::Options> Options(m, "Options");
    Options.def(
        py::init());
    Options.def_property(
        "create_if_missing",
        &Amulet::RocksDB::Options::get_create_if_missing,
        &Amulet::RocksDB::Options::set_create_if_missing);

    py::classh<Amulet::RocksDB::ReadOptions> ReadOptions(m, "ReadOptions");
    ReadOptions.def(
        py::init());

    py::classh<Amulet::RocksDB::WriteOptions> WriteOptions(m, "WriteOptions");
    WriteOptions.def(
        py::init());
    WriteOptions.def_property(
        "sync",
        &Amulet::RocksDB::WriteOptions::get_sync,
        &Amulet::RocksDB::WriteOptions::set_sync);
    WriteOptions.def_property(
        "disable_wal",
        &Amulet::RocksDB::WriteOptions::get_disable_wal,
        &Amulet::RocksDB::WriteOptions::set_disable_wal);

    py::classh<Amulet::RocksDB::CompactRangeOptions> CompactRangeOptions(m, "CompactRangeOptions");
    CompactRangeOptions.def(
        py::init());

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
        py::init([](
                     std::filesystem::path path,
                     Amulet::RocksDB::Options& options,
                     Amulet::RocksDB::ReadOptions& read_options,
                     Amulet::RocksDB::WriteOptions& write_options,
                     Amulet::RocksDB::CompactRangeOptions& compact_range_options) {
            return std::make_unique<Amulet::RocksDB::RocksDB>(
                std::move(path),
                std::move(options),
                std::move(read_options),
                std::move(write_options),
                std::move(compact_range_options));
        }),
        py::arg("path"),
        py::arg("options"),
        py::arg("read_options"),
        py::arg("write_options"),
        py::arg("compact_range_options"),
        py::doc(
            "Construct a new :class:`RocksDB` instance from the database at the given path.\n"
            "\n"
            "A rocksdb database is like a dictionary that only contains bytes as the keys and values and exists entirely on the disk.\n"
            "\n"
            ":param path: The path to the database directory.\n"
            ":param options: The RocksDB Options object.\n"
            ":param read_options: The RocksDB ReadOptions object.\n"
            ":param write_options: The RocksDB WriteOptions object.\n"
            ":param compact_range_options: The RocksDB CompactRangeOptions object.\n"
            ":raises: RocksDBException if an error occured."));

    RocksDB.def(
        "close",
        &Amulet::RocksDB::RocksDB::close,
        py::doc(
            "Close the rocksdb database.\n"
            "Only the owner of the database may close it.\n"
            "If needed, an external lock must be used to ensure that no other threads are accessing the database."),
        py::call_guard<py::gil_scoped_release>());

    RocksDB.def(
        "compact_range",
        [](Amulet::RocksDB::RocksDB& self, std::optional<py::bytes> begin, std::optional<py::bytes> end) {
            std::optional<std::string_view> begin_view;
            std::optional<std::string_view> end_view;
            if (begin) {
                begin_view = begin;
            }
            if (end) {
                end_view = end;
            }
            {
                py::gil_scoped_release gil;
                self.compact_range(begin_view, end_view);
            }
        },
        py::arg("begin"),
        py::arg("end"),
        py::doc("Remove deleted entries from the database to reduce its size."));

    RocksDB.def(
        "compact",
        &Amulet::RocksDB::RocksDB::compact,
        py::doc("Remove deleted entries from the database to reduce its size."),
        py::call_guard<py::gil_scoped_release>());

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
        py::arg("value"),
        py::doc("db[b\"key\"] = b\"value\""));

    RocksDB.def(
        "put_batch",
        [](Amulet::RocksDB::RocksDB& self, pyext::collections::Iterable<std::pair<py::bytes, std::optional<py::bytes>>> py_batch) {
            std::list<std::pair<std::string_view, std::optional<std::string_view>>> batch;
            for (const auto& [key, value] : py_batch) {
                if (value) {
                    batch.emplace_back(key, *value);
                } else {
                    batch.emplace_back(key, std::nullopt);
                }
            }
            {
                py::gil_scoped_release nogil;
                self.put_batch(batch);
            }
        },
        py::arg("batch"),
        py::doc("Set a group of values in the database."));

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
    RocksDB.def(
        "__getitem__",
        get,
        py::arg("key"),
        py::doc("db[b\"key\"]"));

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
            ":param key: The key to delete from the database."));
    RocksDB.def(
        "__delitem__",
        del,
        py::arg("key"),
        py::doc("del db[b\"key\"]"));

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
