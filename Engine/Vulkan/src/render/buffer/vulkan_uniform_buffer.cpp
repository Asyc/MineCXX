#include "engine/vulkan/render/buffer/vulkan_uniform_buffer.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "engine/vulkan/vulkan_context.hpp"

namespace engine::vulkan::render::buffer {

VulkanUniformBuffer::VulkanUniformBuffer(VmaAllocator allocator, size_t size) {
  VkBufferCreateInfo bufferCreateInfo = vk::BufferCreateInfo({}, size, vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::SharingMode::eExclusive);

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
  allocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

  VmaAllocation allocation;
  VkBuffer buffer;
  VmaAllocationInfo allocationInfo;

  vmaCreateBuffer(allocator, &bufferCreateInfo, &allocationCreateInfo, &buffer, &allocation, &allocationInfo);

  m_Buffer = buffer;
  m_Allocation = {allocation, [allocator, buffer](VmaAllocation allocation) {
    vmaDestroyBuffer(allocator, buffer, allocation);
  }};
  m_MappedPtr = allocationInfo.pMappedData;

  VmaAllocatorInfo allocatorInfo;
  vmaGetAllocatorInfo(allocator, &allocatorInfo);
  vk::Device device = allocatorInfo.device;
}

void VulkanUniformBuffer::write(size_t offset, const void* ptr, size_t length) {
  memcpy(reinterpret_cast<char*>(m_MappedPtr) + offset, ptr, length);
}

}   // namespace engine::vulkan::render::buffer