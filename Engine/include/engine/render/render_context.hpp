#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_RENDER_CONTEXT_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_RENDER_CONTEXT_HPP_

#include <memory>

#include "buffer/image.hpp"
#include "buffer/index_buffer.hpp"
#include "buffer/uniform_buffer.hpp"
#include "buffer/vertex_buffer.hpp"
#include "command/command_pool.hpp"
#include "engine/file.hpp"
#include "engine/gui/font/font_renderer.hpp"
#include "engine/gui/input.hpp"

#include "pipeline.hpp"
#include "swapchain.hpp"

namespace engine {
class Window;
}

namespace engine::render {

class RenderContext {
public:
    using ResizeCallback = std::function<void(uint32_t, uint32_t)>;

    virtual ~RenderContext() = default;

    [[nodiscard]] virtual std::shared_ptr<buffer::Image> createImage(const File& path) = 0;

    [[nodiscard]] virtual std::unique_ptr<buffer::VertexBuffer> allocateVertexBuffer(size_t size) = 0;

    [[nodiscard]] virtual std::unique_ptr<buffer::IndexBuffer> allocateIndexBuffer(size_t size) = 0;
    [[nodiscard]] virtual std::unique_ptr<buffer::UniformBuffer> allocateUniformBuffer(const RenderPipeline& pipeline, size_t size) = 0;

    [[nodiscard]] virtual std::shared_ptr<RenderPipeline> createRenderPipeline(const File& file) = 0;

    [[nodiscard]] virtual std::unique_ptr<command::CommandPool> createCommandPool() const = 0;
    [[nodiscard]] virtual command::CommandPool& getThreadCommandPool() = 0;

    [[nodiscard]] virtual Swapchain& getSwapchain() = 0;
    [[nodiscard]] virtual const Swapchain& getSwapchain() const = 0;

    [[nodiscard]] virtual gui::font::FontRenderer& getFontRenderer() = 0;

    [[nodiscard]] virtual const Window& getWindow() const = 0;

    virtual void setResizeCallback(ResizeCallback callback) = 0;

    // Callback functions
    virtual void mouseButtonCallback(gui::input::MouseButton button, gui::input::MouseButtonAction action, double x, double y) = 0;
protected:
    RenderContext() = default;
};

}   // namespace engine::render

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_RENDER_CONTEXT_HPP_
