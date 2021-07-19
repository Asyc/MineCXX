#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_NBT_NBT_PARSER_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_NBT_NBT_PARSER_HPP_

#include <cstdint>

#define BEGIN_NBT_DICTIONARY()
#define END_NBT_DICTIONARY()

namespace engine::nbt {

enum class NbtType {

};

class NbtNode {
public:
    NbtNode(NbtType type);

    void setType(NbtType type);

private:
    NbtType m_Type;
};

class NbtParser {

};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_NBT_NBT_PARSER_HPP_
