#include "engine/vulkan/render/buffer/vulkan_vertex_buffer.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "engine/vulkan/vulkan_context.hpp"

namespace engine::vulkan::render::buffer {

VulkanVertexBuffer::VulkanVertexBuffer(VulkanTransferManager* transferManager, VmaAllocator allocator, size_t size) : m_TransferManager(transferManager) {
    VkBufferCreateInfo bufferCreateInfo = vk::BufferCreateInfo(
        {},
        size,
        vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
        vk::SharingMode::eExclusive
    );

    VmaAllocationCreateInfo allocationCreateInfo{};
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VmaAllocation allocation;
    VkBuffer buffer;
    VmaAllocationInfo allocationInfo;
    vmaCreateBuffer(allocator, &bufferCreateInfo, &allocationCreateInfo, &buffer, &allocation, &allocationInfo);

    m_Buffer = buffer;
    m_Allocation = {allocation, [allocator, buffer](VmaAllocation allocation) {
        vmaDestroyBuffer(allocator, buffer, allocation);
    }};
}

void VulkanVertexBuffer::write(size_t offset, const void* ptr, size_t length) {
    auto transferBuffer = m_TransferManager->getTransferPool().acquireUnique(length);
    transferBuffer->copy(ptr, length, 0);
    m_TransferManager->addTask(std::move(transferBuffer), m_Buffer, 0, 0, length);
}
}   // namespace engine::vulkan::render::buffer