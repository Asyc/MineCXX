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

    virtual void draw(engine::render::command::IDrawableCommandBuffer& buffer) = 0;
private:
    std::vector<std::unique_ptr<Element>> m_Elements;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_GUI_HPP_
