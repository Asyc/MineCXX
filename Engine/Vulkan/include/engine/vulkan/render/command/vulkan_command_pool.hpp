#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_COMMAND_VULKAN_COMMAND_POOL_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_COMMAND_VULKAN_COMMAND_POOL_HPP_

#include "engine/render/command/command_pool.hpp"

#include <vulkan/vulkan.hpp>

#include "engine/vulkan/render/swapchain/vulkan_swapchain.hpp"

namespace engine::render::command::vulkan {

class VulkanCommandPool : public CommandPool {
public:
    VulkanCommandPool(vk::Device device, uint32_t queueFamilyIndex, const render::vulkan::VulkanSwapchain* swapchain);

    std::unique_ptr<DirectCommandBuffer> allocateDirectCommandBuffer() override;
    std::unique_ptr<IndirectCommandBuffer> allocateIndirectCommandBuffer() override;

    std::unique_ptr<CommandList> allocateCommandList() override;
    std::unique_ptr<ImmutableCommandList> allocateCommandListImmutable() override;
private:
    vk::UniqueCommandPool m_CommandPool;

    const render::vulkan::VulkanSwapchain* m_SwapchainHandle;
};

class VulkanImmutableCommandPool : public CommandPool {
public:
    VulkanImmutableCommandPool(vk::Device device, uint32_t queueFamilyIndex, const render::vulkan::VulkanSwapchain* swapchain);

    std::unique_ptr<DirectCommandBuffer> allocateDirectCommandBuffer() override;
    std::unique_ptr<IndirectCommandBuffer> allocateIndirectCommandBuffer() override;

    std::unique_ptr<CommandList> allocateCommandList() override;
    std::unique_ptr<ImmutableCommandList> allocateCommandListImmutable() override;
private:
    vk::UniqueCommandPool m_CommandPool;

    const render::vulkan::VulkanSwapchain* m_SwapchainHandle;
};

}   //namespace engine::render::command::vulkan

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_COMMAND_VULKAN_COMMAND_POOL_HPP_
