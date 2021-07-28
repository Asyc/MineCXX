#include "engine/gui/element/element.hpp"

#include <array>

namespace engine::gui {

ElementImage::ElementImage(engine::render::RenderContext& context, std::shared_ptr<engine::render::buffer::Image> image, float x, float y, float width, float height, const ImageRegion& region) : m_Image(std::move(image)) {
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

    static int invocation = 0;

    float pixelUnitX = 1.0f / static_cast<float>(m_Image->getWidth());
    float pixelUnitY = 1.0f / static_cast<float>(m_Image->getHeight());
    float tX = pixelUnitX * static_cast<float>(region.x);
    float tY = pixelUnitY * static_cast<float>(region.y);
    float tWidth = pixelUnitX * static_cast<float>(region.width);
    float tHeight = pixelUnitY * static_cast<float>(region.height);

    std::array<Vertex, 4> vertices{
        Vertex{x, y, tX, tY},
        Vertex{x, y - height, tX, tY + tHeight},
        Vertex{x + width, y - height, tX + tWidth, tY + tHeight},
        Vertex{x + width, y, tX + tWidth, tY}
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
