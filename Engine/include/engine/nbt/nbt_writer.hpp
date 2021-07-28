#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_NBT_NBT_WRITER_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_NBT_NBT_WRITER_HPP_

#include <cstdint>

#include "nbt_type.hpp"

namespace engine::nbt {

template<NbtType type, typename T>
class NbtSerializer {
public:
    size_t write(T value, char* pOut) = delete;
};

template<>
class NbtSerializer<NbtType::TAG_BYTE, int8_t> {
public:
    size_t write(int8_t value, char* pOut) {
        *pOut = value;
        return 1;
    }
};

template<>
class NbtSerializer<NbtType::TAG_SHORT, int16_t> {
public:
    size_t write(int16_t value, char* pOut) {
        *pOut = value;
        return 1;
    }
};

class NbtWriter {
public:
    void write(N)
};
}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_NBT_NBT_WRITER_HPP_
