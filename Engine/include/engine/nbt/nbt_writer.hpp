#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_NBT_NBT_WRITER_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_NBT_NBT_WRITER_HPP_

#include <cstdint>

namespace engine::nbt {

enum class NbtType : uint8_t {
    TAG_END = 0,
    TAG_BYTE = 1,
    TAG_SHORT = 2,
    TAG_INT = 3,
    TAG_LONG = 4,
    TAG_FLOAT = 5,
    TAG_DOUBLE = 6,
    TAG_BYTE_ARRAY = 7,
    TAG_STRING = 8,
    TAG_LIST = 9,
    TAG_COMPOUND = 10,
    TAG_INT_ARRAY = 11,
    TAG_LONG_ARRAY = 12
};

template <NbtType>
struct NbtNode {};

template <>
struct NbtNode<NbtType::TAG_BYTE> {
    int8_t value;
};

class NbtWriter {
public:
    void write(N)
};
}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_NBT_NBT_WRITER_HPP_
