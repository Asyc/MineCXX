#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_COMMAND_VULKAN_COMMAND_BUFFER_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_COMMAND_VULKAN_COMMAND_BUFFER_HPP_

#include "engine/render/command/command_buffer.hpp"
#include "engine/render/pipeline.hpp"
#include "engine/vulkan/render/swapchain/vulkan_swapchain.hpp"

#include <vulkan/vulkan.hpp>

namespace engine::render::command::vulkan {

class VulkanCommandBuffer : public virtual ICommandBuffer {
public:
    void end() override;

    [[nodiscard]] virtual vk::CommandBuffer getCommandBufferHandle() const = 0;
};

class VulkanDrawableCommandBuffer : public virtual IDrawableCommandBuffer, public virtual VulkanCommandBuffer {
public:
    void bindPipeline(const RenderPipeline& pipeline) override;
};

class VulkanSwitchingCommandBuffer : public virtual VulkanCommandBuffer {
public:

    VulkanSwitchingCommandBuffer(vk::Device device, const render::vulkan::VulkanSwapchain* swapchain, vk::CommandPool pool, vk::CommandBufferLevel level);

    [[nodiscard]] vk::CommandBuffer getCommandBufferHandle() const override;
protected:
    const render::vulkan::VulkanSwapchain* m_SwapchainHandle;
    std::vector<vk::UniqueCommandBuffer> m_Buffers;

};

class VulkanCommandList : public virtual CommandList, public virtual VulkanDrawableCommandBuffer, public virtual VulkanSwitchingCommandBuffer {
public:
    VulkanCommandList(vk::Device device, const render::vulkan::VulkanSwapchain* swapchain, vk::CommandPool pool);
    void begin() override;
};

class VulkanImmutableCommandList : public virtual ImmutableCommandList, public virtual VulkanDrawableCommandBuffer {
public:
    VulkanImmutableCommandList(vk::Device device, const render::vulkan::VulkanSwapchain* swapchain, vk::CommandPool pool);

    void begin() override;

    [[nodiscard]] vk::CommandBuffer getCommandBufferHandle() const override;
private:
    const render::vulkan::VulkanSwapchain* m_SwapchainHandle;

    vk::UniqueCommandBuffer m_CommandBuffer;
};

class VulkanDirectCommandBuffer : public virtual DirectCommandBuffer, public virtual VulkanDrawableCommandBuffer, public virtual VulkanSwitchingCommandBuffer {
public:
    VulkanDirectCommandBuffer(vk::Device device, const render::vulkan::VulkanSwapchain* swapchain, vk::CommandPool pool);
    void begin() override;
    void end() override;
};

class VulkanIndirectCommandBuffer : public virtual IndirectCommandBuffer, public virtual VulkanSwitchingCommandBuffer {
public:
    VulkanIndirectCommandBuffer(vk::Device device, const render::vulkan::VulkanSwapchain* swapchain, vk::CommandPool pool);
    void begin() override;
    void end() override;
};

}   //namespace engine::render::command::vulkan

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_COMMAND_VULKAN_COMMAND_BUFFER_HPP_
