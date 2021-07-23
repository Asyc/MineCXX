#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_FONT_FONT_RENDERER_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_FONT_FONT_RENDERER_HPP_

#include <string>
#include <memory>

#include "engine/file.hpp"
#include "engine/render/buffer/image.hpp"
#include "engine/render/buffer/index_buffer.hpp"

#include "engine/render/command/command_buffer.hpp"

namespace engine::render {
class RenderContext;
}   // namespace engine::render

namespace engine::gui::font {

struct AsciiNode {
    struct {
        float x, y;
    } pos;

    struct {
        float x, y;
    } texPos;
};

class FontRenderer {
public:
    using String = std::u16string;
    using StringView = std::u16string_view;

    FontRenderer(render::RenderContext& context, const File& glyphSizesPath, const Directory& resourceDirectory);

    void draw(render::command::IDrawableCommandBuffer& commandBuffer, const StringView& string);
private:
    std::unique_ptr<render::RenderPipeline> m_Pipeline;
    std::unique_ptr<render::buffer::Image> m_AsciiImage;
    std::unordered_map<uint16_t, std::unique_ptr<render::buffer::VertexBuffer>> m_AsciiFontMap;

    std::unique_ptr<render::buffer::IndexBuffer> m_IndexBuffer;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_FONT_FONT_RENDERER_HPP_
