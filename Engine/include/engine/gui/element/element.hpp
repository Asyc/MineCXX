#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_ELEMENT_ELEMENT_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_ELEMENT_ELEMENT_HPP_

#include "engine/render/render_context.hpp"
#include "engine/render/command/command_buffer.hpp"

namespace engine::gui {

class Element {
public:
    virtual ~Element() = default;

    virtual void draw(engine::render::command::IDrawableCommandBuffer& buffer) = 0;
};

class ElementImage : public Element {
public:
    struct ImageRegion {
        float x, y;
        float width, height;
    };

    ElementImage(engine::render::RenderContext& context, std::shared_ptr<engine::render::buffer::Image> image, float x, float y, float w, float h, const ImageRegion& region = {0.0f, 0.0f, 0.0f, 0.0f});

    void draw(engine::render::command::IDrawableCommandBuffer& buffer) override;
private:
    std::unique_ptr<render::RenderPipeline> m_Pipeline;
    std::unique_ptr<render::UniformDescriptor> m_UniformDescriptorSet;
    std::shared_ptr<render::buffer::Image> m_Image;
    std::unique_ptr<render::buffer::VertexBuffer> m_VertexBuffer;
    std::unique_ptr<render::buffer::IndexBuffer> m_IndexBuffer;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_ELEMENT_ELEMENT_HPP_
