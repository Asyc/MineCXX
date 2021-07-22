#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_ELEMENT_ELEMENT_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_ELEMENT_ELEMENT_HPP_

#include "engine/render/command/command_buffer.hpp"

namespace engine::gui {

class Element {
public:
    virtual ~Element() = default;

    virtual void draw(engine::render::command::IDrawableCommandBuffer& buffer) = 0;

    [[nodiscard]] virtual int getX() const = 0;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_ELEMENT_ELEMENT_HPP_
