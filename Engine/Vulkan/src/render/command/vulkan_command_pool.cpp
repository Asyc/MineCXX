#include "engine/vulkan/render/command/vulkan_command_pool.hpp"

#include <vulkan/vulkan.hpp>

#include "engine/render/pipeline.hpp"
#include "engine/vulkan/render/command/vulkan_command_buffer.hpp"

namespace engine::render::command::vulkan {

VulkanCommandPool::VulkanCommandPool(vk::Device device, uint32_t queueFamilyIndex, const render::vulkan::VulkanSwapchain* swapchain) : m_SwapchainHandle(swapchain) {
    vk::CommandPoolCreateInfo createInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, queueFamilyIndex);
    m_CommandPool = device.createCommandPoolUnique(createInfo);
}

std::unique_ptr<DirectCommandBuffer> vulkan::VulkanCommandPool::allocateDirectCommandBuffer() {
    return std::make_unique<VulkanDirectCommandBuffer>(m_CommandPool.getOwner(), m_SwapchainHandle, *m_CommandPool);
}
std::unique_ptr<IndirectCommandBuffer> vulkan::VulkanCommandPool::allocateIndirectCommandBuffer() {
    return std::make_unique<VulkanIndirectCommandBuffer>(m_CommandPool.getOwner(), m_SwapchainHandle, *m_CommandPool);
}
std::unique_ptr<CommandList> vulkan::VulkanCommandPool::allocateCommandList() {
    return std::make_unique<VulkanCommandList>(m_CommandPool.getOwner(), m_SwapchainHandle, *m_CommandPool);
}
std::unique_ptr<ImmutableCommandList> vulkan::VulkanCommandPool::allocateCommandListImmutable() {
    return std::make_unique<VulkanImmutableCommandList>(m_CommandPool.getOwner(), m_SwapchainHandle, *m_CommandPool);
}

VulkanImmutableCommandPool::VulkanImmutableCommandPool(vk::Device device, uint32_t queueFamilyIndex, const render::vulkan::VulkanSwapchain* swapchain) : m_SwapchainHandle(swapchain) {
    vk::CommandPoolCreateInfo createInfo({}, queueFamilyIndex);
    m_CommandPool = device.createCommandPoolUnique(createInfo);
}
std::unique_ptr<DirectCommandBuffer> VulkanImmutableCommandPool::allocateDirectCommandBuffer() {
    return std::make_unique<VulkanDirectCommandBuffer>(m_CommandPool.getOwner(), m_SwapchainHandle, *m_CommandPool);
}
std::unique_ptr<IndirectCommandBuffer> VulkanImmutableCommandPool::allocateIndirectCommandBuffer() {
    return std::make_unique<VulkanIndirectCommandBuffer>(m_CommandPool.getOwner(), m_SwapchainHandle, *m_CommandPool);
}
std::unique_ptr<CommandList> VulkanImmutableCommandPool::allocateCommandList() {
    return std::make_unique<VulkanCommandList>(m_CommandPool.getOwner(), m_SwapchainHandle, *m_CommandPool);
}
std::unique_ptr<ImmutableCommandList> VulkanImmutableCommandPool::allocateCommandListImmutable() {
    return std::make_unique<VulkanImmutableCommandList>(m_CommandPool.getOwner(), m_SwapchainHandle, *m_CommandPool);
}

}   //namespace engine::render::command::vulkan