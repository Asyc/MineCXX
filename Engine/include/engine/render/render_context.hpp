#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_RENDER_CONTEXT_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_RENDER_CONTEXT_HPP_

#include <memory>

#include "engine/file.hpp"

#include "buffer/index_buffer.hpp"
#include "buffer/vertex_buffer.hpp"
#include "command/command_pool.hpp"

#include "pipeline.hpp"
#include "swapchain.hpp"

namespace engine::render {

class RenderContext {
public:
    virtual ~RenderContext() = default;

    [[nodiscard]] virtual std::unique_ptr<buffer::VertexBuffer> allocateVertexBuffer(size_t size) = 0;
    [[nodiscard]] virtual std::unique_ptr<buffer::IndexBuffer> allocateIndexBuffer(size_t size) = 0;

    [[nodiscard]] virtual std::unique_ptr<RenderPipeline> createRenderPipeline(const File& file) const = 0;

    [[nodiscard]] virtual std::unique_ptr<command::CommandPool> createCommandPool() const = 0;

    [[nodiscard]] virtual Swapchain& getSwapchain() = 0;
    [[nodiscard]] virtual const Swapchain& getSwapchain() const = 0;
protected:
    RenderContext() = default;
};

}   // namespace engine::render

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_RENDER_CONTEXT_HPP_
