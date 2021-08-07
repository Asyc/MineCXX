#include "engine/gui/gui.hpp"

namespace engine::gui {

void Gui::draw(render::command::IDrawableCommandBuffer& buffer) {
  for (const auto& element : m_Elements) {
    element->draw(buffer);
  }
}

void Gui::pushElement(std::unique_ptr<Element> element) {
  m_Elements.push_back(std::move(element));
}

void Gui::onClick(input::MouseButton button, input::MouseButtonAction action, float x, float y) {
  for (auto& element : m_Elements) {
    if (element->isInside(x, y)) element->onClick(button, action, x, y);
  }
}

void Gui::onResize(uint32_t width, uint32_t height) {
  for (auto& element : m_Elements) {
    element->onResize(width, height);
  }
}

}