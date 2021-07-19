#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_COMMAND_VULKAN_COMMAND_BUFFER_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_COMMAND_VULKAN_COMMAND_BUFFER_HPP_

#include "engine/render/command/command_buffer.hpp"

#include <vulkan/vulkan.hpp>

#include "engine/vulkan/render/vulkan_pipeline.hpp"
#include "engine/vulkan/render/swapchain/vulkan_swapchain.hpp"

namespace engine::vulkan::render::command {

using namespace ::engine::render;
using namespace ::engine::render::command;

class VulkanCommandBuffer : public virtual ICommandBuffer {
public:
    void end() override;

    [[nodiscard]] virtual vk::CommandBuffer getCommandBufferHandle() const = 0;
};

class VulkanDrawableCommandBuffer : public virtual IDrawableCommandBuffer, public virtual VulkanCommandBuffer {
public:
    explicit VulkanDrawableCommandBuffer(const VulkanSwapchain* handle);

    void bindPipeline(const RenderPipeline& pipeline) override;

    void bindVertexBuffer(const buffer::VertexBuffer& buffer) override;
    void bindIndexBuffer(const buffer::IndexBuffer& buffer) override;
    void bindUniformBuffer(const buffer::UniformBuffer& buffer, uint32_t set) override;

    void draw(uint32_t instanceCount, uint32_t vertexCount) override;
    void drawIndexed(uint32_t instanceCount, uint32_t indexCount) override;
private:
    const VulkanSwapchain* m_Handle;

    vk::PipelineLayout m_CurrentLayout;
};

class IVulkanSubmittableCommandBuffer : public virtual ISubmittableCommandBuffer {
public:
    [[nodiscard]] virtual vk::Fence getFence() const = 0;
};

class VulkanSwitchingCommandBuffer : public virtual VulkanCommandBuffer, public virtual IVulkanSubmittableCommandBuffer {
public:
    VulkanSwitchingCommandBuffer(vk::Device device, const VulkanSwapchain* swapchain, vk::CommandPool pool, vk::CommandBufferLevel level);
    ~VulkanSwitchingCommandBuffer() override;

    [[nodiscard]] vk::CommandBuffer getCommandBufferHandle() const override;

    [[nodiscard]] vk::Fence getFence() const override;
protected:
    const VulkanSwapchain* m_SwapchainHandle;

    std::vector<vk::UniqueCommandBuffer> m_Buffers;
    std::vector<vk::UniqueFence> m_Fences;
};

class VulkanCommandList : public virtual CommandList, public virtual VulkanDrawableCommandBuffer, public virtual VulkanSwitchingCommandBuffer {
public:
    VulkanCommandList(vk::Device device, const VulkanSwapchain* swapchain, vk::CommandPool pool);
    void begin() override;
};

class VulkanImmutableCommandList : public virtual ImmutableCommandList, public virtual VulkanDrawableCommandBuffer {
public:
    VulkanImmutableCommandList(vk::Device device, const VulkanSwapchain* swapchain, vk::CommandPool pool);

    void begin() override;

    [[nodiscard]] vk::CommandBuffer getCommandBufferHandle() const override;
private:
    const VulkanSwapchain* m_SwapchainHandle;

    vk::UniqueCommandBuffer m_CommandBuffer;
};


class VulkanDirectCommandBuffer : public virtual DirectCommandBuffer, public virtual VulkanDrawableCommandBuffer, public virtual VulkanSwitchingCommandBuffer {
public:
    VulkanDirectCommandBuffer(vk::Device device, const VulkanSwapchain* swapchain, vk::CommandPool pool);
    void begin() override;
    void end() override;
};

class VulkanIndirectCommandBuffer : public virtual IndirectCommandBuffer, public virtual VulkanSwitchingCommandBuffer {
public:
    VulkanIndirectCommandBuffer(vk::Device device, const VulkanSwapchain* swapchain, vk::CommandPool pool);
    void begin() override;
    void end() override;

    void accept(const CommandList& commandList) override;
    void accept(const ImmutableCommandList& commandList) override;
};

}   //namespace engine::vulkan::render::command

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_COMMAND_VULKAN_COMMAND_BUFFER_HPP_
