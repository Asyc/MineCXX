#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_GUI_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_GUI_HPP_

#include <memory>

#include "engine/render/command/command_buffer.hpp"

#include "element/element.hpp"

namespace engine::gui {

enum class MouseButton : int {
    LEFT,
    RIGHT,
    MIDDLE
};

class Gui {
public:
    virtual ~Gui() = default;

    void draw(engine::render::command::IDrawableCommandBuffer& buffer);
    void onClick(input::MouseButton button, input::MouseButtonAction action, float x, float y);
protected:
    Gui() = default;

    void pushElement(std::unique_ptr<Element> element);

    std::vector<std::unique_ptr<Element>> m_Elements;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_GUI_HPP_
