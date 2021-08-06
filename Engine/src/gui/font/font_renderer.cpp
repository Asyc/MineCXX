#include "engine/gui/font/font_renderer.hpp"

#include <vulkan/vulkan.hpp>

#include "engine/gui/font/ascii_font.hpp"
#include "engine/log.hpp"
#include "engine/render/render_context.hpp"
#include "engine/render/viewport.hpp"

namespace engine::gui::font {

struct AsciiVertex {
    struct {
        float x, y, z, w;
    } texPos;

    struct {
        float x, y;
    } pos;

    float padding[2];
};

struct AsciiTable {
    AsciiVertex data[256];
};

StringOptions::StringOptions() : StringOptions(1.0f, 1.0f, 1.0f, 1.0f, false, false) {}
StringOptions::StringOptions(float r, float g, float b, float a, bool center, bool shadow, float scale) : color({r, g, b, a}), scale(scale), center(center), shadow(shadow) {}

FontRenderer::FontRenderer(render::RenderContext& context, const File& glyphSizesPath, const Directory& resourceDirectory) : m_Owner(&context) {
    m_DynamicPipeline = context.createRenderPipeline(File("assets/shaders/font/dynamic"));
    m_CachedPipeline = context.createRenderPipeline(File("assets/shaders/font/cached"));
    m_IndexBuffer = context.allocateIndexBuffer(6);

    std::array<uint32_t, 6> indices = {0, 1, 2, 0, 3, 2};
    m_IndexBuffer->write(0, indices.data(), indices.size());

    m_AsciiTableUniformBuffer = context.allocateUniformBuffer(sizeof(AsciiVertex) * 4 * UINT8_MAX);

    File asciiImage(resourceDirectory.getPath() + "/ascii.png");

    static render::buffer::Image::SamplerOptions samplerOptions(render::buffer::Image::Filter::NEAREST, render::buffer::Image::Filter::NEAREST);
    m_AsciiImage = context.createImage(asciiImage, samplerOptions);
    auto[width, height] = m_AsciiImage->getSize();
    MCE_LOG_DEBUG("ASCII Font Sheet Size: {}x{}", width, height);

    float pixelUnitX = 1.0f / static_cast<float>(width);
    float pixelUnitY = 1.0f / static_cast<float>(height);

    AsciiTable table{};

    for (const auto& node : ASCII_CHAR_SIZES) {
        constexpr float scale = 0.00428f;
        constexpr float FONT_HEIGHT = 8;

        size_t pixelX = node.x * 8;
        size_t pixelY = node.y * 8;

        float tx = pixelUnitX * static_cast<float>(pixelX);
        float ty = pixelUnitY * static_cast<float>(pixelY);

        float tWidth = pixelUnitX * static_cast<float>(node.width);
        float tHeight = pixelUnitY * static_cast<float>(7.5);

        auto horizontalWidth = static_cast<float>(node.width) * scale;
        table.data[resolveIndex(node.value)] = AsciiVertex{{tx, ty, tWidth, tHeight}, {horizontalWidth, 8 * scale}};    // Top-Left
    }

    m_AsciiTableUniformBuffer->write(0, &table, sizeof(AsciiTable));

    m_UniformDescriptorSetCached = m_CachedPipeline->allocateDescriptorSet(0);
    m_UniformDescriptorSetCached->bind(0, *context.getViewport().getUniformBuffer());
    m_UniformDescriptorSetCached->bind(1, *m_AsciiTableUniformBuffer);
    m_UniformDescriptorSetCached->bind(2, *m_AsciiImage);

    m_UniformDescriptorSetDynamic = m_DynamicPipeline->allocateDescriptorSet(0);
    m_UniformDescriptorSetDynamic->bind(0, *context.getViewport().getUniformBuffer());
    m_UniformDescriptorSetDynamic->bind(1, *m_AsciiTableUniformBuffer);
    m_UniformDescriptorSetDynamic->bind(2, *m_AsciiImage);
}

void FontRenderer::drawCached(render::command::IDrawableCommandBuffer& commandBuffer, const FontRenderer::String& string, float x, float y, const StringOptions& renderOptions) {
    constexpr size_t MAX_CHARACTERS = 18;   // Due to total output components available in the geometry shader.
    if (renderOptions.shadow) {
        static StringOptions shadowOptions(56.0f / 255.0f, 56.0f / 255.0f, 56.0f / 255.0f, 1.0f, renderOptions.center);
        constexpr float positionOffsetX = 0.0040f;
        constexpr float positionOffsetY = 0.0035f;

        drawCached(commandBuffer, string, x + positionOffsetX, y - positionOffsetY, shadowOptions);
    }

    commandBuffer.bindPipeline(*m_CachedPipeline);
    commandBuffer.bindUniformDescriptor(*m_UniformDescriptorSetCached);

    auto it = m_StringCache.find(string);
    if (it == m_StringCache.end()) {
        std::unique_ptr<render::buffer::UniformBuffer> buffer = m_Owner->allocateUniformBuffer(18 * sizeof(uint32_t));
        std::unique_ptr<render::UniformDescriptor> uniformDescriptor = m_DynamicPipeline->allocateDescriptorSet(1);

        auto ptr = std::make_unique<uint32_t[]>(string.size() + 1);
        for (size_t i = 0; i < string.length(); i++) {
            ptr[i] = (uint32_t) string[i];
        }

        ptr[string.size()] = 0;

        buffer->write(0, ptr.get(), sizeof(uint32_t) * (string.length() + 1));    // Include null-terminator
        uniformDescriptor->bind(0, *buffer);

        commandBuffer.bindUniformDescriptor(*uniformDescriptor);
        m_StringCache.emplace(string, CachedObject{std::move(buffer), std::move(uniformDescriptor)});
    } else {
        commandBuffer.bindUniformDescriptor(*it->second.uniformDescriptor);
    }

    std::array<float, 2> data = {x, y};
    /**
      * layout (offset = 8) float scale;
      * layout (offset = 12) bool center;
      * layout (offset = 16) vec4 color;
      * layout (offset = 52) uint string_offset;
     */

    struct {
        float scale;
        uint32_t center;
        struct {
            float r, g, b, a;
        } color;
    } pushConstants{renderOptions.scale, renderOptions.center, renderOptions.color.r, renderOptions.color.g, renderOptions.color.b, renderOptions.color.a};

    commandBuffer.pushConstants(render::command::PushConstantUsage::VERTEX, 0, &data[0], sizeof(data));
    commandBuffer.pushConstants(render::command::PushConstantUsage::GEOMETRY, 8, &pushConstants, sizeof(pushConstants));

    size_t drawCalls = std::ceil(string.size() / MAX_CHARACTERS);
    if (drawCalls == 0) drawCalls = 1;

    uint32_t stringIndex = 0;

    for (size_t i = 0; i < drawCalls; i++) {
        uint32_t characters = std::min(MAX_CHARACTERS, string.size() - (i * MAX_CHARACTERS));

        commandBuffer.pushConstants(render::command::PushConstantUsage::GEOMETRY, 52, &stringIndex, sizeof(uint32_t));
        commandBuffer.draw(1, 1);
        stringIndex += characters;
    }
}

void FontRenderer::drawDynamic(render::command::IDrawableCommandBuffer& commandBuffer, const FontRenderer::StringView& string, float x, float y, const StringOptions& renderOptions) {
    if (renderOptions.shadow) {
        static StringOptions shadowOptions(15.0f / 255.0f, 15.0f / 255.0f, 15.0f / 255.0f, 1.0f, renderOptions.center);
        constexpr float positionOffsetX = 0.006;
        constexpr float positionOffsetY = 0.005;

        drawDynamic(commandBuffer, string, x + positionOffsetX, y - positionOffsetY, shadowOptions);
    }

    constexpr size_t MAX_CHARACTERS = 18;   // Due to total output components available in the geometry shader.

    commandBuffer.bindPipeline(*m_DynamicPipeline);
    commandBuffer.bindUniformDescriptor(*m_UniformDescriptorSetDynamic);

    auto it = m_OptionCache.find(renderOptions);
    if (it == m_OptionCache.end()) {
        std::unique_ptr<render::buffer::UniformBuffer> buffer = m_Owner->allocateUniformBuffer(sizeof(StringOptions));
        std::unique_ptr<render::UniformDescriptor> uniformDescriptor = m_DynamicPipeline->allocateDescriptorSet(1);

        buffer->write(0, &renderOptions, sizeof(StringOptions));
        uniformDescriptor->bind(0, *buffer);

        commandBuffer.bindUniformDescriptor(*uniformDescriptor);
        m_OptionCache.emplace(renderOptions, CachedObject{std::move(buffer), std::move(uniformDescriptor)});
    } else {
        commandBuffer.bindUniformDescriptor(*it->second.uniformDescriptor);
    }

    std::array<float, 2> data = {x, y};
    commandBuffer.pushConstants(render::command::PushConstantUsage::VERTEX, 0, data.data(), data.size() * sizeof(float));

    size_t drawCalls = std::ceil(string.size() / MAX_CHARACTERS);
    if (drawCalls == 0) drawCalls = 1;

    std::array<uint32_t, MAX_CHARACTERS> pushConstantBuffer{};

    size_t stringIndex = 0;

    for (size_t i = 0; i < drawCalls; i++) {
        uint32_t characters = std::min(MAX_CHARACTERS, string.size() - (i * MAX_CHARACTERS));
        for (uint32_t j = 0; j < characters; j++) {
            pushConstantBuffer[j] = static_cast<uint16_t>(string[stringIndex++]);
        }

        stringIndex += characters;
        if (characters != MAX_CHARACTERS) pushConstantBuffer[characters] = 0;   // Null-Terminator

        commandBuffer.pushConstants(render::command::PushConstantUsage::GEOMETRY, 8, pushConstantBuffer.data(), pushConstantBuffer.size() * sizeof(uint32_t));

        // 0-8 in Push Constants is Render Origin.
        commandBuffer.draw(1, 1);
    }
}

}