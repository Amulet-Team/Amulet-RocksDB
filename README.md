# Amulet RocksDB

A pybind11 wrapper for RocksDB.


## Install
`pip install amulet-rocksdb`

## Use
```py
from amulet.rocksdb import RocksDB

create_if_missing = True  # optional input. Default False.
db = RocksDB("path/to/db", create_if_missing)
db.put(b"key", b"value")
print(db.get(b"key"))
# b"value"
```

See the [source code](src/amulet/rocksdb/_rocksdb.py.cpp) for full documentation.
