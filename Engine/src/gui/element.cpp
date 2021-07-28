#include "engine/gui/element/element.hpp"

#include <array>

namespace engine::gui {

ElementImage::ElementImage(engine::render::RenderContext& context, std::shared_ptr<engine::render::buffer::Image> image, float x, float y) : m_Image(std::move(image)) {
    m_Pipeline = context.createRenderPipeline(File("assets/shaders/gui/image"));
    m_UniformDescriptorSet = m_Pipeline->allocateDescriptorSet(0);
    m_UniformDescriptorSet->bind(0, *m_Image);

    struct Vertex {
        struct {
            float x, y;
        } pos;
        struct {
            float x, y;
        } texPos;
    };

    float width = 0.601f, height = 0.2f;
    float tX = 0.0f, tY = 0.0f, tWidth = 1.0f, tHeight = 1.0f;

    x -= width / 2.0f;
    y += height * 2;

    std::array<Vertex, 4> vertices {
        Vertex{x, y + height, tX, tY + tHeight},
        Vertex{x, y, tX, tY},
        Vertex{x + width, y, tX + tWidth, tY},
        Vertex{x + width, y + height, tX + tWidth, tY + tHeight}
    };
    constexpr size_t length = sizeof(Vertex) * vertices.size();
    m_VertexBuffer = context.allocateVertexBuffer(length);
    m_VertexBuffer->write(0, vertices.data(), length);

    std::array<uint32_t, 6> indices = {0, 1, 2, 0, 3, 2};
    m_IndexBuffer = context.allocateIndexBuffer(6);
    m_IndexBuffer->write(0, indices.data(), indices.size());
}

void ElementImage::draw(render::command::IDrawableCommandBuffer& buffer) {
    buffer.bindPipeline(*m_Pipeline);
    buffer.bindUniformDescriptor(*m_UniformDescriptorSet);
    buffer.bindVertexBuffer(*m_VertexBuffer);
    buffer.bindIndexBuffer(*m_IndexBuffer);
    buffer.drawIndexed(1, 6);
}

}
