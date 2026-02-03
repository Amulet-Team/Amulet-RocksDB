#pragma once

#ifndef AMULET_ROCKSDB_EXPORT
#ifdef _WIN32
#ifdef ExportAmuletRocksDB
#define AMULET_ROCKSDB_EXPORT __declspec(dllexport)
#else
#define AMULET_ROCKSDB_EXPORT __declspec(dllimport)
#endif
#else
#define AMULET_ROCKSDB_EXPORT
#endif
#endif

#if !defined(AMULET_ROCKSDB_EXPORT_EXCEPTION)
#if defined(_LIBCPP_EXCEPTION)
#define AMULET_ROCKSDB_EXPORT_EXCEPTION __attribute__((visibility("default")))
#else
#define AMULET_ROCKSDB_EXPORT_EXCEPTION
#endif
#endif
