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

    std::array<AsciiVertex, 4> vertexBuffer;

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

                vertexBuffer[0] = AsciiVertex{{0.0f, FONT_HEIGHT}, {tx, ty + tHeight}};    // Top-Left
                vertexBuffer[1] = AsciiVertex{{0.0f, 0.0f}, {tx, ty}};    // Bottom-Left
                vertexBuffer[2] = AsciiVertex{{horizontalWidth, 0.0f}, {tx + tWidth, ty}};    // Bottom-Right
                vertexBuffer[3] = AsciiVertex{{horizontalWidth, FONT_HEIGHT}, {tx + tWidth, ty + tHeight}};    // Top-Right

                auto vbo = context.allocateVertexBuffer(vertexBuffer.size() * sizeof(AsciiVertex));
                vbo->write(0, vertexBuffer.data(), vertexBuffer.size() * sizeof(AsciiVertex));
                m_AsciiFontMap[node.value] = std::move(vbo);

                pixelX += node.width;
            }
        }
    }


}

void FontRenderer::draw(render::command::IDrawableCommandBuffer& commandBuffer, const FontRenderer::StringView& string) {
    for (uint16_t codepoint : string) {
        assert(codepoint < 256);

        const CharNode& data = ASCII_CHAR_SIZES[codepoint];

    }
}

}