#include "engine/gui/font/font_renderer.hpp"

#include "engine/gui/font/ascii_font.hpp"
#include "engine/render/render_context.hpp"

#include <vulkan/vulkan.hpp>

#include "engine/log.hpp"

namespace engine::gui::font {

FontRenderer::FontRenderer(render::RenderContext& context, const File& glyphSizesPath, const Directory& resourceDirectory) {
    m_Pipeline = context.createRenderPipeline(File("assets/shaders/font"));
    m_IndexBuffer = context.allocateIndexBuffer(6);

    std::array<uint32_t, 6> indices = {0, 1, 2, 0, 3, 2};
    m_IndexBuffer->write(0, indices.data(), indices.size());

    for (const auto& file : resourceDirectory.getDirectoryIterator()) {
        if (file.path().filename().string() == "ascii.png") {
            m_AsciiImage = context.createImage(File(file.path().string()));
            auto [width, height] = m_AsciiImage->getSize();

            MCE_LOG_DEBUG("ASCII Font Sheet Size: {}x{}", width, height);

            float tx = 1.0f / m_AsciiImage->getWidth();
            float ty = 1.0f / m_AsciiImage->getHeight();
        }
    }


}

void FontRenderer::draw(render::command::IDrawableCommandBuffer& commandBuffer, const FontRenderer::StringView& string) {

}

}