#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_FONT_FONT_RENDERER_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_FONT_FONT_RENDERER_HPP_

#include <string>
#include <memory>

#include "engine/file.hpp"
#include "engine/render/buffer/image.hpp"
#include "engine/render/buffer/index_buffer.hpp"

#include "engine/render/command/command_buffer.hpp"

namespace engine::gui::font {
struct StringOptions {
    StringOptions();
    StringOptions(float r, float g, float b, float a, bool center = false, bool shadow = false, float scale = 0.00428f);

    struct {
        float r, g, b, a;
    } color;

    float scale;
    uint32_t center;
    uint32_t shadow;    // // This field should not be sent to GPU, shadow handled with a double draw
    float _padding;

    bool operator==(const StringOptions& rhs) const {
        return memcmp(this, &rhs, sizeof(float) * 7) == 0;
    }
};
}

namespace std {
template<>
struct hash<engine::gui::font::StringOptions> {
    std::size_t operator()(const engine::gui::font::StringOptions& k) const {
        using std::size_t;
        using std::hash;
        using std::string;

        std::string hashed = std::to_string(k.color.r) + ':' + std::to_string(k.color.g) + ':' + std::to_string(k.color.b) + ':' + std::to_string(k.color.a) + ':' + std::to_string(k.center);
        return ::std::hash<std::string>().operator()(hashed);
    }
};
}


namespace engine::render {
class RenderContext;
}   // namespace engine::render

namespace engine::gui::font {

class FontRenderer {
public:
    using String = std::u16string;
    using StringView = std::u16string_view;

    FontRenderer(render::RenderContext& context, const File& glyphSizesPath, const Directory& resourceDirectory);

    void drawCached(render::command::IDrawableCommandBuffer& commandBuffer, const String& string, float x, float y, const StringOptions& renderOptions = {});
    void drawDynamic(render::command::IDrawableCommandBuffer& commandBuffer, const StringView& string, float x, float y, const StringOptions& renderOptions = {});

    void draw(render::command::IDrawableCommandBuffer& commandBuffer, const StringView& string, float x, float y, const StringOptions& renderOptions = {}) {
        drawDynamic(commandBuffer, string, x, y, renderOptions);
    }
private:
    struct CachedObject {
        std::unique_ptr<render::buffer::UniformBuffer> buffer;
        std::unique_ptr<render::UniformDescriptor> uniformDescriptor;
    };

    engine::render::RenderContext* m_Owner;

    std::shared_ptr<render::RenderPipeline> m_CachedPipeline, m_DynamicPipeline;

    std::unique_ptr<render::buffer::UniformBuffer> m_AsciiTableUniformBuffer;
    std::shared_ptr<render::buffer::Image> m_AsciiImage;
    std::unique_ptr<render::UniformDescriptor> m_UniformDescriptorSetCached, m_UniformDescriptorSetDynamic;

    std::unique_ptr<render::buffer::IndexBuffer> m_IndexBuffer;

    std::unordered_map<std::u16string, CachedObject> m_StringCache;
    std::unordered_map<StringOptions, CachedObject> m_OptionCache;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_FONT_FONT_RENDERER_HPP_
