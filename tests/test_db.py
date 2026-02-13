import unittest
import struct
from tempfile import TemporaryDirectory
from uuid import uuid4
import glob
import os
import weakref
import time
from concurrent.futures import ThreadPoolExecutor
from collections.abc import Iterable, Sequence

from rocksdb import (
    RocksDB,
    RocksDBException,
    Options,
    WriteOptions,
    CompressionType,
)

num_keys = [struct.pack("<Q", i) for i in range(10_000)]
num_db = dict(zip(num_keys, num_keys))
incr_db = {f"key{i}".encode("utf-8"): f"val{i}".encode("utf-8") for i in range(10_000)}
full_db = {**incr_db, **num_db}


def get_directory_size(path: str) -> int:
    return sum(
        (os.path.getsize(item.path) for item in os.scandir(path) if item.is_file())
    )


class RocksDBTestCase(unittest.TestCase):
    def test_create_ldb(self) -> None:
        with TemporaryDirectory() as path:
            db = RocksDB(path, True)
            db.close()

    def test_create_fail(self) -> None:
        with self.assertRaises(RocksDBException):
            with TemporaryDirectory() as path:
                RocksDB(path)

    def test_read_write(self) -> None:
        with TemporaryDirectory() as path:
            db = RocksDB(path, True)

            with self.assertRaises(KeyError):
                db.get(b"random_key")

            key1 = b"key"
            value1 = b"value"
            db.put(key1, value1)
            self.assertEqual(db.get(key1), value1)

            key2 = key1 * 1000
            value2 = value1 * 1000
            db.put(key2, value2)
            self.assertEqual(db.get(key2), value2)

            db.close()

    def test_put(self) -> None:
        with TemporaryDirectory() as path:
            db = RocksDB(path, True)

            db.put_batch(incr_db.items())

            for k, v in num_db.items():
                db.put(k, v)

            self.assertEqual(set(db), full_db.keys())
            self.assertEqual(set(db.keys()), full_db.keys())
            self.assertEqual(dict(db.items()), full_db)

            db.close()

    def test_get_set_item(self) -> None:
        with TemporaryDirectory() as path:
            db = RocksDB(path, True)

            for k, v in num_db.items():
                db[k] = v

            for k, v in num_db.items():
                self.assertEqual(v, db[k])

            db.close()

    def test_contains(self) -> None:
        with TemporaryDirectory() as path:
            db = RocksDB(path, True)

            self.assertFalse(b"test_key2" in db)

            db.put(b"test_key2", b"test")

            self.assertTrue(b"test_key2" in db)

            db.close()

    def test_delete(self) -> None:
        with TemporaryDirectory() as path:
            db = RocksDB(path, True)

            self.assertFalse(b"test_key3" in db)

            db.put(b"test_key3", b"test")

            self.assertTrue(b"test_key3" in db)

            db.delete(b"test_key3")

            self.assertFalse(b"test_key3" in db)

            db[b"test_key3"] = b"test"

            self.assertTrue(b"test_key3" in db)

            del db[b"test_key3"]

            self.assertFalse(b"test_key3" in db)

            db.close()

    def test_exception(self) -> None:
        with TemporaryDirectory() as path:
            db = RocksDB(path, True)
            db.close()

            # if the db is closed all of these functions should error
            # they should not cause segmentation faults
            db.close()  # This should do nothing.
            with self.assertRaises(RuntimeError):
                db.compact()
            with self.assertRaises(RuntimeError):
                db.get(b"key")
            with self.assertRaises(RuntimeError):
                _ = db[b"key"]
            with self.assertRaises(RuntimeError):
                db.put(b"key", b"value")
            with self.assertRaises(RuntimeError):
                db[b"key"] = b"value"
            with self.assertRaises(RuntimeError):
                db.put_batch(((b"key", b"value"),))
            with self.assertRaises(RuntimeError):
                db.delete(b"key")
            with self.assertRaises(RuntimeError):
                del db[b"key"]
            with self.assertRaises(RuntimeError):
                list(db.keys())
            with self.assertRaises(RuntimeError):
                list(db.values())
            with self.assertRaises(RuntimeError):
                list(db.items())
            with self.assertRaises(RuntimeError):
                _ = b"key" in db
            with self.assertRaises(RuntimeError):
                list(db)

    def test_iterate_twice(self) -> None:
        with TemporaryDirectory() as path:
            db = RocksDB(path, True)
            db.put_batch(
                (
                    (b"a", b"1"),
                    (b"b", b"2"),
                    (b"c", b"3"),
                    (b"d", b"4"),
                    (b"e", b"5"),
                    (b"f", b"6"),
                )
            )

            it1 = db.items()
            self.assertEqual((b"a", b"1"), next(it1))
            self.assertEqual((b"b", b"2"), next(it1))
            self.assertEqual((b"c", b"3"), next(it1))
            it2 = db.items()
            self.assertEqual((b"a", b"1"), next(it2))
            self.assertEqual((b"d", b"4"), next(it1))
            self.assertEqual((b"b", b"2"), next(it2))

            db.close()

    def test_keys_twice(self) -> None:
        with TemporaryDirectory() as path:
            db = RocksDB(path, True)
            db.put_batch(
                (
                    (b"a", b"1"),
                    (b"b", b"2"),
                    (b"c", b"3"),
                    (b"d", b"4"),
                    (b"e", b"5"),
                    (b"f", b"6"),
                )
            )

            it1 = db.keys()
            self.assertEqual(b"a", next(it1))
            self.assertEqual(b"b", next(it1))
            self.assertEqual(b"c", next(it1))
            it2 = db.keys()
            self.assertEqual(b"a", next(it2))
            self.assertEqual(b"d", next(it1))
            self.assertEqual(b"b", next(it2))

            db.close()

    def test_iter_mutate(self) -> None:
        with TemporaryDirectory() as path:
            db = RocksDB(path, True)
            db.put_batch(
                (
                    (b"a", b"1"),
                    (b"b", b"2"),
                    (b"c", b"3"),
                    (b"d", b"4"),
                    (b"e", b"5"),
                    (b"f", b"6"),
                )
            )

            it = db.items()
            self.assertEqual((b"a", b"1"), next(it))
            self.assertEqual((b"b", b"2"), next(it))
            self.assertEqual((b"c", b"3"), next(it))
            db.put(b"d", b"10")
            self.assertEqual(b"10", db.get(b"d"))
            self.assertEqual((b"d", b"4"), next(it))
            self.assertEqual((b"e", b"5"), next(it))
            self.assertEqual((b"f", b"6"), next(it))
            with self.assertRaises(StopIteration):
                next(it)

            self.assertEqual(
                {
                    b"a": b"1",
                    b"b": b"2",
                    b"c": b"3",
                    b"d": b"10",
                    b"e": b"5",
                    b"f": b"6",
                },
                dict(db),
            )

            db.close()

    def test_lock(self) -> None:
        with TemporaryDirectory() as path:
            db = RocksDB(path, True)
            db.close()

            manifest_paths = glob.glob(os.path.join(path, "MANIFEST-*"))
            self.assertEqual(1, len(manifest_paths))
            manifest_path = manifest_paths[0]

            db = RocksDB(path, True)
            try:
                manifest_paths = glob.glob(os.path.join(path, "MANIFEST-*"))
                self.assertEqual(1, len(manifest_paths))
                self.assertNotEqual(manifest_path, manifest_paths[0])
            finally:
                db.close()

    def test_compact(self) -> None:
        with TemporaryDirectory() as path:
            db = RocksDB(path, True)
            try:
                for _ in range(500_000):
                    key = str(uuid4()).encode()
                    db.put(key, key)
            finally:
                db.close()

            self.assertGreater(get_directory_size(path), 1_000_000)

            db = RocksDB(path)
            try:
                for key in db.keys():
                    db.delete(key)
                db.compact()
            finally:
                db.close()

            self.assertLess(get_directory_size(path), 200_000)

    def test_corrupt(self) -> None:
        """Test how the library handles a corrupt db."""
        with TemporaryDirectory() as path:
            db = RocksDB(path, True)
            try:
                for _ in range(1_000_000):
                    key = str(uuid4()).encode()
                    db.put(key, key)
            finally:
                db.close()

            # delete one of the ldb files
            os.remove(next(glob.iglob(os.path.join(glob.escape(path), "*.log"))))

            db = RocksDB(path, True)
            try:
                key_count = len(list(db.keys()))
                self.assertLessEqual(key_count, 1_000_000)
                self.assertGreater(key_count, 10)
            finally:
                db.close()

    def test_iterator_lifespan(self) -> None:
        with TemporaryDirectory() as path:
            db = RocksDB(path, True)
            db_ref = weakref.ref(db)

            # Set values
            db[b"1"] = b"1"
            db[b"2"] = b"2"
            db[b"3"] = b"3"

            # Get iterators
            it_k = db.keys()
            it_v = db.values()
            it_i = db.items()

            # Close database
            db.close()

            with self.assertRaises(RuntimeError):
                next(it_k)
            with self.assertRaises(RuntimeError):
                next(it_v)
            with self.assertRaises(RuntimeError):
                next(it_i)

            del db

            with self.assertRaises(RuntimeError):
                next(it_k)
            with self.assertRaises(RuntimeError):
                next(it_v)
            with self.assertRaises(RuntimeError):
                next(it_i)

            self.assertIs(None, db_ref())

    def test_iterator(self) -> None:
        with TemporaryDirectory() as path:
            db = RocksDB(path, True)

            it1 = db.items()
            it1.seek_to_first()
            self.assertFalse(it1.valid())
            with self.assertRaises(RuntimeError):
                it1.key()
            with self.assertRaises(RuntimeError):
                it1.value()

            # Set values
            db[b"1"] = b"2"
            db[b"3"] = b"4"
            db[b"5"] = b"6"

            # The first iterator should remain invalid
            with self.assertRaises(RuntimeError):
                it1.key()
            with self.assertRaises(RuntimeError):
                it1.value()

            it1.seek_to_first()
            self.assertFalse(it1.valid())
            with self.assertRaises(RuntimeError):
                it1.key()
            with self.assertRaises(RuntimeError):
                it1.value()

            it2 = db.items()
            it2.seek_to_first()
            self.assertTrue(it2.valid())
            self.assertEqual(b"1", it2.key())
            self.assertEqual(b"2", it2.value())
            it2.next()
            self.assertTrue(it2.valid())
            self.assertEqual(b"3", it2.key())
            self.assertEqual(b"4", it2.value())
            it2.next()
            self.assertTrue(it2.valid())
            self.assertEqual(b"5", it2.key())
            self.assertEqual(b"6", it2.value())
            it2.prev()
            self.assertTrue(it2.valid())
            self.assertEqual(b"3", it2.key())
            self.assertEqual(b"4", it2.value())
            it2.next()
            it2.next()
            self.assertFalse(it2.valid())

            it2.seek_to_last()
            self.assertTrue(it2.valid())
            self.assertEqual(b"5", it2.key())
            self.assertEqual(b"6", it2.value())

            it2.seek(b"2")
            self.assertTrue(it2.valid())
            self.assertEqual(b"3", it2.key())
            self.assertEqual(b"4", it2.value())

            db.close()

    def test_thread_write(self) -> None:
        count = 10_000
        data = [
            [struct.pack(">i", i + j) for j in range(count)]
            for i in range(0, count * 10, count)
        ]
        m = {k: k for k in sum(data, [])}

        with TemporaryDirectory() as path:
            t1 = -time.time()
            db = RocksDB(path, True)
            for l in data:
                for v in l:
                    db.put(v, v)
            db.close()
            t1 += time.time()

            db = RocksDB(path)
            m1 = dict(db)
            db.close()
            self.assertEqual(m, m1)

        with TemporaryDirectory() as path:
            t2 = -time.time()
            db = RocksDB(path, True)

            def add(values: Iterable[bytes]) -> None:
                for v in values:
                    db.put(v, v)

            with ThreadPoolExecutor() as executor:
                executor.map(add, data)

            db.close()
            t2 += time.time()

            db = RocksDB(path)
            m2 = dict(db)
            db.close()
            self.assertEqual(m, m2)

    def test_thread_read(self) -> None:
        count = 10_000
        data = [
            [struct.pack(">i", i + j) for j in range(count)]
            for i in range(0, count * 10, count)
        ]
        m = {k: k for k in sum(data, [])}

        with TemporaryDirectory() as path:
            db = RocksDB(path, True)

            def add(values: Iterable[bytes]) -> None:
                for v in values:
                    db.put(v, v)

            with ThreadPoolExecutor() as executor:
                executor.map(add, data)

            db.close()

            # Validate the database
            db = RocksDB(path)
            m2 = dict(db)
            db.close()
            self.assertEqual(m, m2)

            # # Read serial
            db = RocksDB(path)
            m1 = {}
            t1 = -time.time()
            it = db.items()
            it.seek_to_first()
            while it.valid():
                m1[it.key()] = it.value()
                it.next()
            t1 += time.time()
            db.close()
            self.assertEqual(m, m1)

            # Read parallel
            db = RocksDB(path)
            m2 = {}
            t2 = -time.time()

            def read(values: Sequence[bytes]) -> None:
                it = db.items()
                it.seek(values[0])
                for _ in range(count):
                    m2[it.key()] = it.value()
                    it.next()

            with ThreadPoolExecutor() as executor:
                executor.map(read, data)

            t2 += time.time()
            db.close()
            self.assertEqual(m, m2)

    def test_disable_wal(self) -> None:
        # Test that disabling the Write Ahead Log (WAL) is actually faster
        data = [struct.pack(">Q", i) * 8 for i in range(100_000)]

        def write_read(wal: bool) -> float:
            with TemporaryDirectory() as path:
                options = Options()
                options.create_if_missing = True
                options.compression_type = CompressionType.ZstdCompression
                write_options = WriteOptions()
                if not wal:
                    write_options.sync = False
                    write_options.disable_wal = True
                db = RocksDB(
                    path,
                    options=options,
                    write_options=write_options,
                )
                t1 = time.perf_counter()
                for d in data:
                    db.put(d, d)
                db.close()
                t2 = time.perf_counter()

                db2 = RocksDB(path)
                self.assertEqual(dict(zip(data, data)), dict(db2.items()))
                db2.close()

            return t2 - t1

        self.assertLess(write_read(False), write_read(True))


if __name__ == "__main__":
    unittest.main()
