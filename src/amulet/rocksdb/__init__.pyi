from __future__ import annotations

import os

from . import _rocksdb, _version

__all__: list[str] = ["RocksDB", "RocksDBException", "compiler_config"]

class RocksDB:
    """
    A RocksDB database
    """

    def __delitem__(self, key: bytes) -> None: ...
    def __getitem__(self, key: bytes) -> bytes: ...
    def __init__(
        self, path: os.PathLike | str | bytes, create_if_missing: bool = False
    ) -> None:
        """
        Construct a new :class:`RocksDB` instance from the database at the given path.

        A rocksdb database is like a dictionary that only contains bytes as the keys and values and exists entirely on the disk.

        :param path: The path to the database directory.
        :param create_if_missing: If True a new database will be created if one does not exist at the given path.
        :raises: RocksDBException if an error occured.
        """

    def __setitem__(self, key: bytes, value: bytes) -> None: ...
    def close(self) -> None:
        """
        Close the rocksdb database.
        Only the owner of the database may close it.
        If needed, an external lock must be used to ensure that no other threads are accessing the database.
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

    def put(self, key: bytes, value: bytes) -> None:
        """
        Set a value in the database.
        """

class RocksDBException(Exception):
    pass

def _init() -> None: ...

__version__: str
compiler_config: dict
