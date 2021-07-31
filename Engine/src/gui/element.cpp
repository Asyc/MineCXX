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

    float pixelUnitX = 1.0f / static_cast<float>(m_Image->getWidth());
    float pixelUnitY = 1.0f / static_cast<float>(m_Image->getHeight());
    float tX = pixelUnitX * static_cast<float>(0.35f);
    float tY = pixelUnitY * static_cast<float>(66);
    float tWidth = pixelUnitX * static_cast<float>(199.45f);
    float tHeight = pixelUnitY * static_cast<float>(20);

    m_TextureData.standard[0] = {tX, tY};
    m_TextureData.standard[1] = {tX, tY + tHeight};
    m_TextureData.standard[2] = {tX + tWidth, tY + tHeight};
    m_TextureData.standard[3] = {tX + tWidth, tY};

    tY += tHeight;

    m_TextureData.hover[0] = {tX, tY};
    m_TextureData.hover[1] = {tX, tY + tHeight};
    m_TextureData.hover[2] = {tX + tWidth, tY + tHeight};
    m_TextureData.hover[3] = {tX + tWidth, tY};
}

void ElementButton::draw(render::command::IDrawableCommandBuffer& buffer) {
    buffer.bindPipeline(*m_Pipeline);
    buffer.bindUniformDescriptor(*m_UniformDescriptorSet);
    buffer.bindVertexBuffer(*m_VertexBuffer);
    buffer.bindIndexBuffer(*m_IndexBuffer);

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

    const auto* pushConstantData = xAlign && yAlign ? m_TextureData.hover : m_TextureData.standard;

    buffer.pushConstants(render::command::PushConstantUsage::VERTEX, 0, pushConstantData, sizeof(TexturePositionData::Vertex) * 4);
    buffer.drawIndexed(1, 6);

    if (!m_Text.empty()) {
        float x = m_Position.w / 2.0f + m_Position.x;
        float y = m_Position.y - m_Position.h / 4.0f;

        engine::gui::font::StringOptions stringOptions(1.0f, 1.0f, 1.0f, 1.0f, true, true);

        m_Context->getFontRenderer().drawDynamic(buffer, m_Text, x, y, stringOptions);
    }
}

}
