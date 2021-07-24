#include "engine/gui/font/font_renderer.hpp"

#include "engine/gui/font/ascii_font.hpp"
#include "engine/render/render_context.hpp"

#include <vulkan/vulkan.hpp>

#include "engine/log.hpp"

namespace engine::gui::font {

struct AsciiVertex {
    struct {
        float x, y;
    } pos;

    struct {
        float x, y;
    } texPos;
};

FontRenderer::FontRenderer(render::RenderContext& context, const File& glyphSizesPath, const Directory& resourceDirectory) {
    m_Pipeline = context.createRenderPipeline(File("assets/shaders/font"));
    m_IndexBuffer = context.allocateIndexBuffer(6);

    std::array<uint32_t, 6> indices = {0, 1, 2, 0, 3, 2};
    m_IndexBuffer->write(0, indices.data(), indices.size());

    m_AsciiTableUniformBuffer = context.allocateUniformBuffer(*m_Pipeline, sizeof(AsciiVertex) * 4 * UINT8_MAX);

    std::array<std::array<AsciiVertex, 4>, 256> vertexData{};

    for (const auto& file : resourceDirectory.getDirectoryIterator()) {
        if (file.path().filename().string() == "ascii.png") {
            m_AsciiImage = context.createImage(File(file.path().string()));
            auto [width, height] = m_AsciiImage->getSize();

            MCE_LOG_DEBUG("ASCII Font Sheet Size: {}x{}", width, height);

            float pixelUnitX = 1.0f / static_cast<float>(width);
            float pixelUnitY = 1.0f / static_cast<float>(height);

            size_t pixelX = 0;
            for (const auto& node : ASCII_CHAR_SIZES) {
                if (node.x == 0) pixelX = 0;
                size_t pixelY = node.y * 8;

                float tx = pixelUnitX * static_cast<float>(pixelX);
                float ty = pixelUnitY * static_cast<float>(pixelY);

                constexpr float FONT_HEIGHT = 8;

                float tWidth = pixelUnitX * static_cast<float>(node.width);
                float tHeight = pixelUnitY * static_cast<float>(FONT_HEIGHT);

                auto horizontalWidth = static_cast<float>(node.width);

                vertexData[node.value][0] = AsciiVertex{{0.0f, FONT_HEIGHT}, {tx, ty + tHeight}};    // Top-Left
                vertexBuffer[1] = AsciiVertex{{0.0f, 0.0f}, {tx, ty}};    // Bottom-Left
                vertexBuffer[2] = AsciiVertex{{horizontalWidth, 0.0f}, {tx + tWidth, ty}};    // Bottom-Right
                vertexBuffer[3] = AsciiVertex{{horizontalWidth, FONT_HEIGHT}, {tx + tWidth, ty + tHeight}};    // Top-Right

                m_AsciiTableUniformBuffer->write(node.value * sizeof(AsciiVertex), vertexBuffer.data(), vertexBuffer.size() * sizeof(AsciiVertex));

                pixelX += node.width;
            }
        }
    }

    m_UniformDescriptorSet = m_Pipeline->allocateDescriptorSet(0);
    m_UniformDescriptorSet->bind(0, *m_AsciiTableUniformBuffer);
    m_UniformDescriptorSet->bind(1, *m_AsciiImage);
}

template <typename T>
inline T roundUp(T numToRound, T multiple) {
    if (multiple == 0)
        return numToRound;

    T remainder = numToRound % multiple;
    if (remainder == 0)
        return numToRound;

    return numToRound + multiple - remainder;
}

void FontRenderer::draw(render::command::IDrawableCommandBuffer& commandBuffer, const FontRenderer::StringView& string) {
    constexpr size_t MAX_CHARACTERS = 60;

    commandBuffer.bindPipeline(*m_Pipeline);
    commandBuffer.bindUniformDescriptor(*m_UniformDescriptorSet);

    std::array<float, 2> data = {0.0f, 0.0f};
    commandBuffer.pushConstants(0, data.data(), data.size() * sizeof(float));

    size_t drawCalls = string.size() / MAX_CHARACTERS;
    if (drawCalls == 0) drawCalls = 1;

    std::array<char16_t, MAX_CHARACTERS> buffer{};
    memcpy(buffer.data(), string.data(), std::min(MAX_CHARACTERS, string.size()));

    size_t lastPushConstantWrite;

    float currentX, currentY = 0.0f;

    std::array<uint8_t, 128> pushConstantBuffer{};

    for (size_t i = 0; i < drawCalls; i++) {
        uint32_t characters = std::min(MAX_CHARACTERS, string.size() - (i * MAX_CHARACTERS));

        lastPushConstantWrite = roundUp<uint32_t>(characters, 4);
        commandBuffer.pushConstants(sizeof(float) * 2, (void*) (string.data() + (i * MAX_CHARACTERS)), lastPushConstantWrite);
        commandBuffer.draw(1, 4 * characters);

        memcpy(buffer.data(), string.data() + (i * MAX_CHARACTERS) + characters, characters);


    }


}

}