#include "engine/gui/font/font_renderer.hpp"

#include "engine/gui/font/ascii_font.hpp"
#include "engine/render/render_context.hpp"

#include <vulkan/vulkan.hpp>

#include "engine/log.hpp"

namespace engine::gui::font {

struct AsciiVertex {
    struct {
        float x, y, z, w;
    } texPos;

    struct {
        float x, y;
    } pos;
};

struct AsciiTable {
    AsciiVertex data[256];
};

FontRenderer::FontRenderer(render::RenderContext& context, const File& glyphSizesPath, const Directory& resourceDirectory) {
    m_Pipeline = context.createRenderPipeline(File("assets/shaders/font"));
    m_IndexBuffer = context.allocateIndexBuffer(6);

    std::array<uint32_t, 6> indices = {0, 1, 2, 0, 3, 2};
    m_IndexBuffer->write(0, indices.data(), indices.size());

    m_AsciiTableUniformBuffer = context.allocateUniformBuffer(*m_Pipeline, sizeof(AsciiVertex) * 4 * UINT8_MAX);

    File asciiImage(resourceDirectory.getPath() + "/ascii.png");

    m_AsciiImage = context.createImage(asciiImage);
    auto[width, height] = m_AsciiImage->getSize();
    MCE_LOG_DEBUG("ASCII Font Sheet Size: {}x{}", width, height);

    float pixelUnitX = 1.0f / static_cast<float>(width);
    float pixelUnitY = 1.0f / static_cast<float>(height);

    size_t pixelX = 0;
    AsciiTable table{};

    for (const auto& node : ASCII_CHAR_SIZES) {
        if (node.x == 0) pixelX = 0;
        size_t pixelY = node.y * 8;

        float tx = pixelUnitX * static_cast<float>(pixelX);
        float ty = pixelUnitY * static_cast<float>(pixelY);

        constexpr float FONT_HEIGHT = 8;

        float tWidth = pixelUnitX * static_cast<float>(node.width);
        float tHeight = pixelUnitY * static_cast<float>(FONT_HEIGHT);

        auto horizontalWidth = static_cast<float>(node.width);
        if (node.value == u'F') {
            int i = 0;
        }
        table.data[resolveIndex(node.value)] = AsciiVertex{{tx, ty, tWidth, tHeight}, {horizontalWidth, FONT_HEIGHT}};    // Top-Left
        pixelX += node.width;
    }

    m_AsciiTableUniformBuffer->write(0, &table, sizeof(AsciiTable));

    m_UniformDescriptorSet = m_Pipeline->allocateDescriptorSet(0);
    m_UniformDescriptorSet->bind(0, *m_AsciiTableUniformBuffer);
    m_UniformDescriptorSet->bind(1, *m_AsciiImage);
}

template<typename T>
inline T roundUp(T numToRound, T multiple) {
    if (multiple == 0)
        return numToRound;

    T remainder = numToRound % multiple;
    if (remainder == 0)
        return numToRound;

    return numToRound + multiple - remainder;
}

void FontRenderer::draw(render::command::IDrawableCommandBuffer& commandBuffer, const FontRenderer::StringView& string) {
    constexpr size_t MAX_CHARACTERS = 28;

    commandBuffer.bindPipeline(*m_Pipeline);
    commandBuffer.bindUniformDescriptor(*m_UniformDescriptorSet);

    std::array<float, 2> data = {0.0f, 0.0f};
    commandBuffer.pushConstants(render::command::PushConstantUsage::VERTEX, 0, data.data(), data.size() * sizeof(float));

    size_t drawCalls = std::ceil(string.size() / MAX_CHARACTERS);
    if (drawCalls == 0) drawCalls = 1;

    std::array<uint32_t, 112 / 4> pushConstantBuffer{};

    size_t stringIndex = 0;

    for (size_t i = 0; i < drawCalls; i++) {
        uint32_t characters = std::min(MAX_CHARACTERS, string.size() - (i * MAX_CHARACTERS));
        for (uint32_t j = 0; j < characters; j++) {
            pushConstantBuffer[j] = static_cast<uint16_t>(string.data()[stringIndex++]);
        }

        stringIndex += characters;
        if (characters < pushConstantBuffer.size()) pushConstantBuffer[characters] = 0; // Null-Terminator

        commandBuffer.pushConstants(render::command::PushConstantUsage::GEOMETRY, 8, pushConstantBuffer.data(), pushConstantBuffer.size() * sizeof(uint32_t));
        commandBuffer.draw(1, 1);
    }

}

}