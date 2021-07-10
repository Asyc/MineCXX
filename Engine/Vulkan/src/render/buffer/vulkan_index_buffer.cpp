#include "engine/vulkan/render/buffer/vulkan_index_buffer.hpp"

#include "engine/vulkan/vulkan_context.hpp"

namespace engine::render::buffer::vulkan {

VulkanIndexBuffer::VulkanIndexBuffer(vk::Device device, uint32_t memoryTypeIndex, size_t size, engine::render::vulkan::VulkanRenderContext* context, VulkanTransferPool* transferPool) : m_Context(context), m_TransferPool(transferPool) {
    vk::BufferCreateInfo bufferCreateInfo(
        {},
        size * sizeof(uint32_t),
        vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
        vk::SharingMode::eExclusive
    );
    m_Buffer = device.createBufferUnique(bufferCreateInfo);

    vk::MemoryRequirements requirements = device.getBufferMemoryRequirements(*m_Buffer);

    vk::MemoryAllocateInfo allocateInfo(requirements.size, memoryTypeIndex);
    m_Allocation = device.allocateMemoryUnique(allocateInfo);

    device.bindBufferMemory(*m_Buffer, *m_Allocation, 0);
}

void VulkanIndexBuffer::write(size_t offset, const uint32_t* ptr, size_t length) {
    auto transferBuffer = m_TransferPool->acquireUnique(length * sizeof(uint32_t));
    transferBuffer->copy(ptr, length * sizeof(uint32_t), 0);

    vk::BufferCopy copy(0, offset * sizeof(uint32_t), length * sizeof(uint32_t));
    m_Context->getSwapchainVulkan().addTransfer(std::move(transferBuffer), *m_Buffer, copy);
}

vk::Buffer VulkanIndexBuffer::getBuffer() const {
    return *m_Buffer;
}

}   // namespace engine::render::buffer::vulkan