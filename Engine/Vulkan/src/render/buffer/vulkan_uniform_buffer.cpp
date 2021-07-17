#include "engine/vulkan/render/buffer/vulkan_uniform_buffer.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "engine/vulkan/vulkan_context.hpp"

namespace engine::vulkan::render::buffer {

VulkanUniformBuffer::VulkanUniformBuffer(VmaAllocator allocator, size_t size) {
    VkBufferCreateInfo bufferCreateInfo = vk::BufferCreateInfo({}, size, vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::SharingMode::eExclusive);

    VmaAllocationCreateInfo allocationCreateInfo{};
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VmaAllocation allocation;
    VkBuffer buffer;
    VmaAllocationInfo allocationInfo;

    vmaCreateBuffer(allocator, &bufferCreateInfo, &allocationCreateInfo, &buffer, &allocation, &allocationInfo);

    m_Buffer = buffer;
    m_Allocation = {allocation, [allocator, buffer](VmaAllocation allocation){
        vmaDestroyBuffer(allocator, buffer, allocation);
    }};
}

void VulkanUniformBuffer::write(size_t offset, const void* ptr, size_t length) {

}

VulkanDirectUniformBuffer::VulkanDirectUniformBuffer(VulkanTransferManager* transferManager, VmaAllocator allocator, size_t size) {
    VkBufferCreateInfo bufferCreateInfo = vk::BufferCreateInfo({}, size, vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::SharingMode::eExclusive);

    VmaAllocationCreateInfo allocationCreateInfo{};
    allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    VmaAllocation allocation;
    VkBuffer buffer;
    VmaAllocationInfo allocationInfo;

    vmaCreateBuffer(allocator, &bufferCreateInfo, &allocationCreateInfo, &buffer, &allocation, &allocationInfo);

    m_Buffer = buffer;
    m_Allocation = {allocation, [allocator, buffer](VmaAllocation allocation){
        vmaDestroyBuffer(allocator, buffer, allocation);
    }};
    m_MappedPtr = allocationInfo.pMappedData;
}

void VulkanDirectUniformBuffer::write(size_t offset, const void* ptr, size_t length) {
    if (m_Backing.has_value()) {
        m_Backing->write(offset, ptr, length);
    } else {
        memcpy(reinterpret_cast<char*>(m_MappedPtr) + offset, ptr, length);
    }
}

}   // namespace engine::vulkan::render::buffer