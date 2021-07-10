#include "engine/vulkan/render/command/vulkan_command_buffer.hpp"

#include "engine/vulkan/render/buffer/vulkan_vertex_buffer.hpp"
#include "engine/vulkan/render/vulkan_pipeline.hpp"

namespace engine::render::command::vulkan {

void VulkanCommandBuffer::end() {
    getCommandBufferHandle().end();
}

VulkanDrawableCommandBuffer::VulkanDrawableCommandBuffer(const render::vulkan::VulkanSwapchain* handle) : m_Handle(handle) {}

vk::CommandBuffer VulkanImmutableCommandList::getCommandBufferHandle() const {
    return *m_CommandBuffer;
}

void VulkanDrawableCommandBuffer::bindPipeline(const RenderPipeline& pipeline) {
    const auto* vkPipeline = reinterpret_cast<const render::vulkan::VulkanRenderPipeline*>(&pipeline);
    getCommandBufferHandle().bindPipeline(vk::PipelineBindPoint::eGraphics, vkPipeline->getPipeline());

    vk::Rect2D scissor({}, m_Handle->getExtent());
    getCommandBufferHandle().setScissor(0, 1, &scissor);

    vk::Viewport viewport(0.0f, 0.0f, static_cast<float>(scissor.extent.width), static_cast<float>(scissor.extent.height), 0.0f, 1.0f);
    getCommandBufferHandle().setViewport(0, 1, &viewport);
}

void VulkanDrawableCommandBuffer::bindVertexBuffer(const buffer::VertexBuffer& buffer) {
    const auto* vkBuffer = reinterpret_cast<const buffer::vulkan::VulkanVertexBuffer*>(&buffer);

    vk::Buffer bufferHandle = vkBuffer->getBuffer();
    size_t offset = 0;
    getCommandBufferHandle().bindVertexBuffers(0, 1, &bufferHandle, &offset);
}

void VulkanDrawableCommandBuffer::draw(uint32_t instanceCount, uint32_t vertexCount) {
    getCommandBufferHandle().draw(vertexCount, instanceCount, 0, 0);
}

VulkanSwitchingCommandBuffer::VulkanSwitchingCommandBuffer(vk::Device device, const render::vulkan::VulkanSwapchain* swapchain, vk::CommandPool pool, vk::CommandBufferLevel level) : m_SwapchainHandle(swapchain), m_Fences(swapchain->getFrameCount()) {
    vk::CommandBufferAllocateInfo allocateInfo(pool, level, m_SwapchainHandle->getFrameCount());
    m_Buffers = device.allocateCommandBuffersUnique(allocateInfo);

    for (auto& m_Fence : m_Fences) {
        m_Fence = device.createFenceUnique({});
    }
}

VulkanSwitchingCommandBuffer::~VulkanSwitchingCommandBuffer() {
    for (const auto& fence : m_Fences) {
        fence.getOwner().waitForFences(1, &*fence, VK_FALSE, UINT64_MAX);
    }
}

vk::CommandBuffer VulkanSwitchingCommandBuffer::getCommandBufferHandle() const {
    return *m_Buffers[m_SwapchainHandle->getCurrentFrameIndex()];
}

vk::Fence VulkanSwitchingCommandBuffer::getFence() const {
    const auto& fence = m_Fences[m_SwapchainHandle->getCurrentFrameIndex()];
    fence.getOwner().resetFences(1, &fence.operator*());
    return *fence;
}

void VulkanCommandList::begin() {
    vk::CommandBufferInheritanceInfo inheritanceInfo(m_SwapchainHandle->getRenderPass(), 0, {});
    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit | vk::CommandBufferUsageFlagBits::eRenderPassContinue, &inheritanceInfo);
    getCommandBufferHandle().begin(beginInfo);
}

VulkanImmutableCommandList::VulkanImmutableCommandList(vk::Device device, const render::vulkan::VulkanSwapchain* swapchain, vk::CommandPool pool) : VulkanDrawableCommandBuffer(swapchain), m_SwapchainHandle(swapchain) {
    vk::CommandBufferAllocateInfo allocateInfo(pool, vk::CommandBufferLevel::eSecondary, 1);

    m_CommandBuffer = std::move(device.allocateCommandBuffersUnique(allocateInfo)[0]);
}

void VulkanImmutableCommandList::begin() {
    vk::CommandBufferInheritanceInfo inheritanceInfo(m_SwapchainHandle->getRenderPass(), 0, {});
    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse | vk::CommandBufferUsageFlagBits::eRenderPassContinue, &inheritanceInfo);
    getCommandBufferHandle().begin(beginInfo);
}

VulkanCommandList::VulkanCommandList(vk::Device device, const render::vulkan::VulkanSwapchain* swapchain, vk::CommandPool pool) : VulkanDrawableCommandBuffer(swapchain), VulkanSwitchingCommandBuffer(device, swapchain, pool, vk::CommandBufferLevel::eSecondary) {

}

VulkanDirectCommandBuffer::VulkanDirectCommandBuffer(vk::Device device, const render::vulkan::VulkanSwapchain* swapchain, vk::CommandPool pool) : VulkanDrawableCommandBuffer(swapchain), VulkanSwitchingCommandBuffer(device, swapchain, pool, vk::CommandBufferLevel::ePrimary) {

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

void VulkanIndirectCommandBuffer::accept(const CommandList& commandList) {
    const auto* buffer = dynamic_cast<const VulkanCommandBuffer*>(&commandList);

    vk::CommandBuffer handle = buffer->getCommandBufferHandle();
    getCommandBufferHandle().executeCommands(1, &handle);
}

void VulkanIndirectCommandBuffer::accept(const ImmutableCommandList& commandList) {
    const auto* buffer = dynamic_cast<const VulkanCommandBuffer*>(&commandList);

    vk::CommandBuffer handle = buffer->getCommandBufferHandle();
    getCommandBufferHandle().executeCommands(1, &handle);
}

}   //namespace engine::render::command::vulkan