#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_NBT_NBT_TYPE_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_NBT_NBT_TYPE_HPP_

#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>

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

class NbtCompound;

struct NbtNode {
    NbtType type;
    int32_t length;
    std::variant<int8_t, int16_t, int32_t, int64_t, float, double, int8_t*, std::string, NbtCompound,
};

class NbtCompound {
public:
    [[nodiscard]] const std::string& getName() const {return m_Name;}
private:
    std::string m_Name;
    std::unordered_map<std::string, NbtNode> m_Values;
};

class NbtList {
public:

private:
    std::vector<NbtNode> m_Values;
};
}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_NBT_NBT_TYPE_HPP_
