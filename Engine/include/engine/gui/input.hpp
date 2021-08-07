#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_INPUT_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_INPUT_HPP_

#include <cstdint>
#include <functional>

namespace engine::gui::input {

enum class MouseButton {
  LEFT,
  RIGHT,
  MIDDLE
};

enum class MouseButtonAction {
  PRESS,
  RELEASE
};

using MouseButtonCallback = std::function<void(MouseButton button, MouseButtonAction action)>;
}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_INPUT_HPP_
