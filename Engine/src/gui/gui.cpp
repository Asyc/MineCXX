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

}