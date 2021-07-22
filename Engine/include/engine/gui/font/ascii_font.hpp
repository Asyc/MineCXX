#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_FONT_ASCII_FONT_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_FONT_ASCII_FONT_HPP_

#include <array>
#include <cstdint>
#include <string>

namespace engine::gui::font {

struct CharNode {
    uint16_t value;
    size_t x, y;
    size_t width;
};

extern std::array<CharNode, 256> ASCII_CHAR_SIZES;

}   // namespace engine::font::font

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_FONT_ASCII_FONT_HPP_
