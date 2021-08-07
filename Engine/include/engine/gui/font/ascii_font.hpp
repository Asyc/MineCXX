#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_FONT_ASCII_FONT_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_FONT_ASCII_FONT_HPP_

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <stdexcept>

#include <unordered_map>

namespace engine::gui::font {

extern std::array<struct CharNode, 256> ASCII_CHAR_SIZES;
extern std::unique_ptr<std::unordered_map<char16_t, uint8_t>> ASCII_INDEXES;

uint8_t map(int8_t value);

struct CharNode {
  uint16_t value;
  size_t x, y;
  size_t width;
};

inline uint8_t resolveIndex(char16_t character) {
  auto it = ASCII_INDEXES->find(character);
  if (it == ASCII_INDEXES->end()) {
    throw std::runtime_error("index out of range");
  }

  return it->second;
}

}   // namespace engine::font::font

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_FONT_ASCII_FONT_HPP_
