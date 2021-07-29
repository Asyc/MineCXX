#include "engine/gui/element/element.hpp"

#include <array>

#include "engine/window.hpp"

namespace engine::gui {

ElementImage::ElementImage(engine::render::RenderContext& context, std::shared_ptr<engine::render::buffer::Image> image, float x, float y, float width, float height, const ImageRegion& region, const Options& options) : m_Image(std::move(image)) {
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

    m_Options = options;
}

void ElementImage::draw(render::command::IDrawableCommandBuffer& buffer) {
    buffer.bindPipeline(*m_Pipeline);
    buffer.pushConstants(render::command::PushConstantUsage::FRAGMENT, 0, &m_Options, sizeof(Options));

    buffer.bindUniformDescriptor(*m_UniformDescriptorSet);
    buffer.bindVertexBuffer(*m_VertexBuffer);
    buffer.bindIndexBuffer(*m_IndexBuffer);
    buffer.drawIndexed(1, 6);
}

ElementButton::ElementButton(render::RenderContext& context, float x, float y, float w, float h) : m_Context(&context) {
    m_Image = context.createImage(NamespaceFile("minecraft", "textures/gui/widgets.png"));
    m_Pipeline = context.createRenderPipeline(File("assets/shaders/gui/button"));
    m_UniformDescriptorSet = m_Pipeline->allocateDescriptorSet(0);
    m_UniformDescriptorSet->bind(0, *m_Image);

    struct Vertex{
        float x, y;
    };

    std::array<Vertex, 4> vertices{
        Vertex{x, y},
        Vertex{x, y - h},
        Vertex{x + w, y - h},
        Vertex{x + w, y}
    };

    constexpr size_t length = sizeof(Vertex) * vertices.size();
    m_VertexBuffer = context.allocateVertexBuffer(length);
    m_VertexBuffer->write(0, vertices.data(), length);

    std::array<uint32_t, 6> indices = {0, 1, 2, 0, 3, 2};
    m_IndexBuffer = context.allocateIndexBuffer(6);
    m_IndexBuffer->write(0, indices.data(), indices.size());

    m_Position = {x, y, w, h};
}

float normalize(float min, float max, float value) {
    float range = max - min;
    return (value - min) / range;
}

void ElementButton::draw(render::command::IDrawableCommandBuffer& buffer) {
    buffer.bindPipeline(*m_Pipeline);
    buffer.bindUniformDescriptor(*m_UniformDescriptorSet);
    buffer.bindVertexBuffer(*m_VertexBuffer);
    buffer.bindIndexBuffer(*m_IndexBuffer);

    struct {
        struct {
            float x, y;
            float padding[2];
        } texPos[4];
    } pushConstants{};

    float pixelUnitX = 1.0f / static_cast<float>(m_Image->getWidth());
    float pixelUnitY = 1.0f / static_cast<float>(m_Image->getHeight());
    float tX = pixelUnitX * static_cast<float>(0);
    float tY = pixelUnitY * static_cast<float>(66);
    float tWidth = pixelUnitX * static_cast<float>(200);
    float tHeight = pixelUnitY * static_cast<float>(20);

    auto [mouseX, mouseY] = m_Context->getWindow().getCursorPosition();
    auto [windowWidth, windowHeight] = m_Context->getSwapchain().getSize();

    float normalizedMouseX = static_cast<float>(mouseX) / static_cast<float>(windowWidth);
    float normalizedMouseY = static_cast<float>(mouseY) / static_cast<float>(windowHeight);

    float viewportX = -1.0f;
    float viewportWidth = 2.0f;

    float viewportY = 1.0f;
    float viewportHeight = 2.0f;

    float viewportMouseX = viewportX + (viewportWidth * normalizedMouseX);
    float viewportMouseY = viewportY - (viewportHeight * normalizedMouseY);

    bool xAlign = viewportMouseX >= m_Position.x && viewportMouseX <= m_Position.x + m_Position.w;
    bool yAlign = viewportMouseY <= m_Position.y && viewportMouseY >= m_Position.y - m_Position.h;
    if (xAlign && yAlign) {
        tY += tHeight;
    }

    pushConstants.texPos[0] = {tX, tY};
    pushConstants.texPos[1] = {tX, tY + tHeight};
    pushConstants.texPos[2] = {tX + tWidth, tY + tHeight};
    pushConstants.texPos[3] = {tX + tWidth, tY};

    buffer.pushConstants(render::command::PushConstantUsage::VERTEX, 0, &pushConstants, sizeof(pushConstants));
    buffer.drawIndexed(1, 6);
}

}
