from __future__ import annotations

import collections.abc
import os
import typing

from . import _rocksdb, _version

__all__: list[str] = [
    "BaseIterator",
    "CompactRangeOptions",
    "CompressionType",
    "ItemsIterator",
    "KeysIterator",
    "Options",
    "ReadOptions",
    "RocksDB",
    "RocksDBException",
    "ValuesIterator",
    "WriteOptions",
    "compiler_config",
]

class BaseIterator:
    def __iter__(self) -> typing.Any: ...
    def key(self) -> bytes:
        """
        Get the key of the current entry in the database.
        :raises: runtime_error if iterator is not valid.
        """

    def next(self) -> None:
        """
        Seek to the next entry in the database.
        """

    def prev(self) -> None:
        """
        Seek to the previous entry in the database.
        """

    def seek(self, target: bytes) -> None:
        """
        Seek to the given entry in the database.
        If the entry does not exist it will seek to the location after.
        """

    def seek_to_first(self) -> None:
        """
        Seek to the first entry in the database.
        """

    def seek_to_last(self) -> None:
        """
        Seek to the last entry in the database.
        """

    def valid(self) -> bool:
        """
        Is the iterator at a valid entry.
        If False, calls to other methods may error.
        """

    def value(self) -> bytes:
        """
        Get the value of the current entry in the database.
        :raises: runtime_error if iterator is not valid.
        """

class CompactRangeOptions:
    def __init__(self) -> None: ...

class CompressionType:
    """
    Members:

      NoCompression : No compression.

      ZstdCompression : Zstd compression.
    """

    NoCompression: typing.ClassVar[
        CompressionType
    ]  # value = amulet.rocksdb.CompressionType.NoCompression
    ZstdCompression: typing.ClassVar[
        CompressionType
    ]  # value = amulet.rocksdb.CompressionType.ZstdCompression
    __members__: typing.ClassVar[
        dict[str, CompressionType]
    ]  # value = {'NoCompression': amulet.rocksdb.CompressionType.NoCompression, 'ZstdCompression': amulet.rocksdb.CompressionType.ZstdCompression}
    def __eq__(self, other: typing.Any) -> bool: ...
    def __hash__(self) -> int: ...
    def __index__(self) -> int: ...
    def __init__(self, value: typing.SupportsInt) -> None: ...
    def __int__(self) -> int: ...
    def __ne__(self, other: typing.Any) -> bool: ...
    def __repr__(self) -> str: ...
    def __str__(self) -> str: ...
    @property
    def name(self) -> str: ...
    @property
    def value(self) -> int: ...

class ItemsIterator(BaseIterator):
    def __next__(self) -> tuple[bytes, bytes]: ...

class KeysIterator(BaseIterator):
    def __next__(self) -> bytes: ...

class Options:
    compression_type: CompressionType
    create_if_missing: bool
    def __init__(self) -> None: ...

class ReadOptions:
    def __init__(self) -> None: ...

class RocksDB:
    """
    A RocksDB database
    """

    def __contains__(self, key: bytes) -> bool: ...
    def __delitem__(self, key: bytes) -> None:
        """
        del db[b"key"]
        """

    def __getitem__(self, key: bytes) -> bytes:
        """
        db[b"key"]
        """

    @typing.overload
    def __init__(
        self,
        path: os.PathLike | str | bytes,
        create_if_missing: bool = False,
        compression_type: CompressionType = ...,
    ) -> None:
        """
        Construct a new :class:`RocksDB` instance from the database at the given path.

        A rocksdb database is like a dictionary that only contains bytes as the keys and values and exists entirely on the disk.

        :param path: The path to the database directory.
        :param create_if_missing: If True a new database will be created if one does not exist at the given path.
        :param compression_type: The compression type to use. (Default ZStandardCompression)
        :raises: RocksDBException if an error occured.
        """

    @typing.overload
    def __init__(
        self,
        path: os.PathLike | str | bytes,
        options: Options,
        read_options: ReadOptions,
        write_options: WriteOptions,
        compact_range_options: CompactRangeOptions,
    ) -> None:
        """
        Construct a new :class:`RocksDB` instance from the database at the given path.

        A rocksdb database is like a dictionary that only contains bytes as the keys and values and exists entirely on the disk.

        :param path: The path to the database directory.
        :param options: The RocksDB Options object.
        :param read_options: The RocksDB ReadOptions object.
        :param write_options: The RocksDB WriteOptions object.
        :param compact_range_options: The RocksDB CompactRangeOptions object.
        :raises: RocksDBException if an error occured.
        """

    def __iter__(self) -> KeysIterator: ...
    def __setitem__(self, key: bytes, value: bytes) -> None:
        """
        db[b"key"] = b"value"
        """

    def close(self) -> None:
        """
        Close the rocksdb database.
        Only the owner of the database may close it.
        If needed, an external lock must be used to ensure that no other threads are accessing the database.
        """

    def compact(self) -> None:
        """
        Remove deleted entries from the database to reduce its size.
        """

    def compact_range(self, begin: bytes | None, end: bytes | None) -> None:
        """
        Remove deleted entries from the database to reduce its size.
        """

    def delete(self, key: bytes) -> None:
        """
        Delete a key from the database.

        :param key: The key to delete from the database.
        """

    def get(self, key: bytes) -> bytes:
        """
        Get a key from the database.

        :param key: The key to get from the database.
        :return: The data stored behind the given key.
        :raises: KeyError if the requested key is not present.
        :raises: RocksDBException on other error.
        """

    def items(self) -> ItemsIterator:
        """
        An iterable of all items in the database.
        """

    def keys(self) -> KeysIterator:
        """
        An iterable of all keys in the database.
        """

    def put(self, key: bytes, value: bytes) -> None:
        """
        Set a value in the database.
        """

    def put_batch(
        self, batch: collections.abc.Iterable[tuple[bytes, bytes | None]]
    ) -> None:
        """
        Set a group of values in the database.
        """

    def values(self) -> ValuesIterator:
        """
        An iterable of all values in the database.
        """

class RocksDBException(Exception):
    pass

class ValuesIterator(BaseIterator):
    def __next__(self) -> bytes: ...

class WriteOptions:
    disable_wal: bool
    sync: bool
    def __init__(self) -> None: ...

def _init() -> None: ...

__version__: str
compiler_config: dict
