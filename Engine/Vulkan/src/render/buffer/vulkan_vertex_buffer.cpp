#include "engine/vulkan/render/buffer/vulkan_vertex_buffer.hpp"

#include "engine/vulkan/vulkan_context.hpp"

namespace engine::render::buffer::vulkan {

VulkanVertexBuffer::VulkanVertexBuffer(vk::Device device, uint32_t memoryTypeIndex, size_t size, engine::render::vulkan::VulkanRenderContext* context, VulkanTransferPool* transferPool) : m_Context(context), m_TransferPool(transferPool) {
    vk::BufferCreateInfo bufferCreateInfo(
        {},
        size,
        vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
        vk::SharingMode::eExclusive
    );
    m_Buffer = device.createBufferUnique(bufferCreateInfo);

    vk::MemoryRequirements requirements = device.getBufferMemoryRequirements(*m_Buffer);

    vk::MemoryAllocateInfo allocateInfo(requirements.size, memoryTypeIndex);
    m_Allocation = device.allocateMemoryUnique(allocateInfo);

    device.bindBufferMemory(*m_Buffer, *m_Allocation, 0);
}

void VulkanVertexBuffer::write(size_t offset, const void* ptr, size_t length) {
    auto transferBuffer = m_TransferPool->acquireUnique(length);
    transferBuffer->copy(ptr, length, 0);

    vk::BufferCopy copy(0, offset, length);
    m_Context->getSwapchainVulkan().addTransfer(std::move(transferBuffer), *m_Buffer, copy);
}

vk::Buffer VulkanVertexBuffer::getBuffer() const {
    return *m_Buffer;
}

}   // namespace engine::render::buffer::vulkan