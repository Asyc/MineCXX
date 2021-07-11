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
    if (m_MemoryAlloc) m_MemoryAlloc.getOwner().unmapMemory(*m_MemoryAlloc);
}

void VulkanTransferBuffer::copy(const void* src, size_t length, size_t offset) const {
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

VulkanTransferPool::VulkanTransferPool(vk::Device owner, uint32_t memoryTypeIndex) : m_Owner(owner), m_MemoryTypeIndex(memoryTypeIndex) {

}

VulkanTransferBuffer* VulkanTransferPool::acquire(size_t minimumSize) {
    std::unique_lock<std::mutex> lock(m_Mutex);

    auto it = m_Available.begin();
    while (it != m_Available.end()) {
        VulkanTransferBuffer* element = it.operator*();

        if (element->getSize() >= minimumSize) {
            m_Available.erase(it);
            return element;
        }

        it++;
    }

    auto& ref = m_Buffers.emplace_back(m_Owner, m_MemoryTypeIndex, minimumSize);
    return &ref;
}


void VulkanTransferPool::release(VulkanTransferBuffer* element) {
    std::unique_lock<std::mutex> lock(m_Mutex);
    m_Available.push_back(element);
}

VulkanTransferBufferUnique VulkanTransferPool::acquireUnique(size_t minimumSize) {
    auto* acquired = acquire(minimumSize);
    return {this, acquired};
}

VulkanTransferBufferUnique::VulkanTransferBufferUnique(VulkanTransferPool* owner, VulkanTransferBuffer* buffer) : m_Owner(owner), m_Buffer(buffer) {

}

VulkanTransferBufferUnique::VulkanTransferBufferUnique(VulkanTransferBufferUnique&& rhs) noexcept : m_Owner(nullptr), m_Buffer(nullptr) {
    std::swap(m_Owner, rhs.m_Owner);
    std::swap(m_Buffer, rhs.m_Buffer);
}

VulkanTransferBufferUnique::~VulkanTransferBufferUnique() {
    if (m_Owner != nullptr && m_Buffer != nullptr) {
        m_Owner->release(m_Buffer);
    }
}

VulkanTransferBufferUnique& VulkanTransferBufferUnique::operator=(VulkanTransferBufferUnique&& rhs) noexcept {
    if (this != &rhs) {
        std::swap(m_Owner, rhs.m_Owner);
        std::swap(m_Buffer, rhs.m_Buffer);
    }

    return *this;
}

}   // namespace engine::render::buffer::vulkan