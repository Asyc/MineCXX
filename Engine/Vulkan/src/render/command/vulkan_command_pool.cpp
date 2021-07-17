#include "engine/vulkan/render/command/vulkan_command_pool.hpp"

#include "engine/vulkan/render/command/vulkan_command_buffer.hpp"

namespace engine::vulkan::render::command {

VulkanCommandPool::VulkanCommandPool(vk::Device device, uint32_t queueFamilyIndex, bool transient, const VulkanSwapchain* swapchain) : m_SwapchainHandle(swapchain) {
    vk::CommandPoolCreateInfo createInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, queueFamilyIndex);
    if (transient) createInfo.flags |= vk::CommandPoolCreateFlagBits::eTransient;
    m_CommandPool = device.createCommandPoolUnique(createInfo);
}

std::unique_ptr<DirectCommandBuffer> VulkanCommandPool::allocateDirectCommandBuffer() {
    return std::make_unique<VulkanDirectCommandBuffer>(m_CommandPool.getOwner(), m_SwapchainHandle, *m_CommandPool);
}
std::unique_ptr<IndirectCommandBuffer> VulkanCommandPool::allocateIndirectCommandBuffer() {
    return std::make_unique<VulkanIndirectCommandBuffer>(m_CommandPool.getOwner(), m_SwapchainHandle, *m_CommandPool);
}
std::unique_ptr<CommandList> VulkanCommandPool::allocateCommandList() {
    return std::make_unique<VulkanCommandList>(m_CommandPool.getOwner(), m_SwapchainHandle, *m_CommandPool);
}
std::unique_ptr<ImmutableCommandList> VulkanCommandPool::allocateCommandListImmutable() {
    return std::make_unique<VulkanImmutableCommandList>(m_CommandPool.getOwner(), m_SwapchainHandle, *m_CommandPool);
}

vk::CommandPool VulkanCommandPool::getCommandPool() const {
    return *m_CommandPool;
}

}   //namespace engine::vulkan::render::command