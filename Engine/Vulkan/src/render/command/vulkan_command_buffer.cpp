#include "engine/vulkan/render/command/vulkan_command_buffer.hpp"

#include "engine/vulkan/render/vulkan_pipeline.hpp"

namespace engine::render::command::vulkan {

void VulkanCommandBuffer::end() {
    getCommandBufferHandle().end();
}


vk::CommandBuffer VulkanImmutableCommandList::getCommandBufferHandle() const {
    return *m_CommandBuffer;
}

void VulkanDrawableCommandBuffer::bindPipeline(const RenderPipeline& pipeline) {
    const auto* vkPipeline = reinterpret_cast<const render::vulkan::VulkanRenderPipeline*>(&pipeline);
    getCommandBufferHandle().bindPipeline(vk::PipelineBindPoint::eGraphics, vkPipeline->getPipeline());
}

VulkanSwitchingCommandBuffer::VulkanSwitchingCommandBuffer(vk::Device device, const render::vulkan::VulkanSwapchain* swapchain, vk::CommandPool pool, vk::CommandBufferLevel level) : m_SwapchainHandle(swapchain) {
    vk::CommandBufferAllocateInfo allocateInfo(pool, level, m_SwapchainHandle->getFrameCount());
    m_Buffers = device.allocateCommandBuffersUnique(allocateInfo);
}

vk::CommandBuffer VulkanSwitchingCommandBuffer::getCommandBufferHandle() const {
    return *m_Buffers[m_SwapchainHandle->getCurrentFrameIndex()];
}

void VulkanCommandList::begin() {
    vk::CommandBufferInheritanceInfo inheritanceInfo(m_SwapchainHandle->getRenderPass(), 0, m_SwapchainHandle->getCurrentFrame());
    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit | vk::CommandBufferUsageFlagBits::eRenderPassContinue, &inheritanceInfo);
    getCommandBufferHandle().begin(beginInfo);
}

VulkanImmutableCommandList::VulkanImmutableCommandList(vk::Device device, const render::vulkan::VulkanSwapchain* swapchain, vk::CommandPool pool) : m_SwapchainHandle(swapchain) {
    vk::CommandBufferAllocateInfo allocateInfo(pool, vk::CommandBufferLevel::eSecondary, 1);

    m_CommandBuffer = std::move(device.allocateCommandBuffersUnique(allocateInfo)[0]);
}

void VulkanImmutableCommandList::begin() {
    vk::CommandBufferInheritanceInfo inheritanceInfo(m_SwapchainHandle->getRenderPass(), 0, m_SwapchainHandle->getCurrentFrame());
    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse | vk::CommandBufferUsageFlagBits::eRenderPassContinue, &inheritanceInfo);
    getCommandBufferHandle().begin(beginInfo);
}

VulkanCommandList::VulkanCommandList(vk::Device device, const render::vulkan::VulkanSwapchain* swapchain, vk::CommandPool pool) : VulkanSwitchingCommandBuffer(device, swapchain, pool, vk::CommandBufferLevel::eSecondary) {

}

VulkanDirectCommandBuffer::VulkanDirectCommandBuffer(vk::Device device, const render::vulkan::VulkanSwapchain* swapchain, vk::CommandPool pool) : VulkanSwitchingCommandBuffer(device, swapchain, pool, vk::CommandBufferLevel::ePrimary) {

}

void VulkanDirectCommandBuffer::begin() {
    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    getCommandBufferHandle().begin(beginInfo);

    auto [w, h] = m_SwapchainHandle->getSize();
    vk::RenderPassBeginInfo renderPassBeginInfo(m_SwapchainHandle->getRenderPass(), m_SwapchainHandle->getCurrentFrame(), vk::Rect2D({}, {w, h}));
    getCommandBufferHandle().beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
}

void VulkanDirectCommandBuffer::end() {
    getCommandBufferHandle().endRenderPass();
    VulkanCommandBuffer::end();
}

void VulkanIndirectCommandBuffer::begin() {
    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    getCommandBufferHandle().begin(beginInfo);

    auto [w, h] = m_SwapchainHandle->getSize();
    vk::RenderPassBeginInfo renderPassBeginInfo(m_SwapchainHandle->getRenderPass(), m_SwapchainHandle->getCurrentFrame(), vk::Rect2D({}, {w, h}));
    getCommandBufferHandle().beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eSecondaryCommandBuffers);
}

void VulkanIndirectCommandBuffer::end() {
    getCommandBufferHandle().endRenderPass();
    VulkanCommandBuffer::end();
}

VulkanIndirectCommandBuffer::VulkanIndirectCommandBuffer(vk::Device device, const render::vulkan::VulkanSwapchain* swapchain, vk::CommandPool pool) : VulkanSwitchingCommandBuffer(device, swapchain, pool, vk::CommandBufferLevel::ePrimary) {

}

}   //namespace engine::render::command::vulkan