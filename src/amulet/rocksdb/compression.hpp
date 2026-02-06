#pragma once

namespace Amulet {
namespace RocksDB {
    enum class CompressionType : unsigned char {
        NoCompression = 0x00,
        ZStandardCompression = 0x07,
    };
} // namespace RocksDB
} // namespace Amulet
