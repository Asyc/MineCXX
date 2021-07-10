#include "engine/vulkan/render/buffer/vulkan_transfer_pool.hpp"

namespace engine::render::buffer::vulkan {

VulkanTransferBuffer::VulkanTransferBuffer(vk::Device device, uint32_t memoryTypeIndex, size_t size) : m_Size(size) {
    vk::BufferCreateInfo bufferCreateInfo({}, size, vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst, vk::SharingMode::eExclusive);
    m_Buffer = device.createBufferUnique(bufferCreateInfo);

    vk::MemoryRequirements requirements = device.getBufferMemoryRequirements(*m_Buffer);

    vk::MemoryAllocateInfo memoryAllocateInfo(requirements.size, memoryTypeIndex);
    m_MemoryAlloc = device.allocateMemoryUnique(memoryAllocateInfo);

    device.bindBufferMemory(*m_Buffer, *m_MemoryAlloc, 0);

    m_MappedPtr = device.mapMemory(*m_MemoryAlloc, 0, size);
}

VulkanTransferBuffer::~VulkanTransferBuffer() {
    m_Buffer.getOwner().unmapMemory(*m_MemoryAlloc);
}

void VulkanTransferBuffer::copy(void* src, size_t length, size_t offset) const {
    memcpy(reinterpret_cast<int8_t*>(m_MappedPtr) + offset, src, length);
}

void VulkanTransferBuffer::write(vk::CommandBuffer buffer, size_t dstOffset, vk::Buffer dst, size_t length) const {
    vk::BufferCopy copy(0, dstOffset, length);
    buffer.copyBuffer(*m_Buffer, dst, 1, &copy);
}

size_t VulkanTransferBuffer::getSize() const {
    return m_Size;
}

vk::Buffer VulkanTransferBuffer::getBuffer() const {
    return *m_Buffer;
}

VulkanTransferPool::VulkanTransferPool(vk::Device owner, size_t memoryTypeIndex) : m_Owner(owner), m_MemoryTypeIndex(memoryTypeIndex) {

}

VulkanTransferBuffer* VulkanTransferPool::acquire(size_t minimumSize) {
    std::unique_lock<std::mutex> lock(m_Mutex);

    for (VulkanTransferBuffer* element : m_Available) {
        if (element->getSize() >= minimumSize) return element;
    }

    auto& element = m_Buffers.emplace_back(m_Owner, m_MemoryTypeIndex, minimumSize);
    return &element;
}

void VulkanTransferPool::release(VulkanTransferBuffer* element) {
    std::unique_lock<std::mutex> lock(m_Mutex);
    m_Available.push_back(element);
}

}   // namespace engine::render::buffer::vulkan