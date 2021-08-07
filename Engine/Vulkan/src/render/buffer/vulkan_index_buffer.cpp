#include "engine/vulkan/render/buffer/vulkan_index_buffer.hpp"

#include "engine/vulkan/vulkan_context.hpp"

namespace engine::vulkan::render::buffer {

VulkanIndexBuffer::VulkanIndexBuffer(VulkanTransferManager* transferManager, VmaAllocator allocator, size_t size) : m_TransferManager(transferManager) {
  VkBufferCreateInfo bufferCreateInfo = vk::BufferCreateInfo(
      {},
      size * sizeof(uint32_t),
      vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
      vk::SharingMode::eExclusive
  );

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  VmaAllocation allocation;
  VkBuffer buffer;
  vmaCreateBuffer(allocator, &bufferCreateInfo, &allocationCreateInfo, &buffer, &allocation, &m_AllocationInfo);

  m_Buffer = buffer;
  m_Allocation = {allocation, [allocator, buffer](VmaAllocation allocation) {
    vmaDestroyBuffer(allocator, buffer, allocation);
  }};
}

void VulkanIndexBuffer::write(size_t offset, const uint32_t* ptr, size_t length) {
  size_t size = length * sizeof(uint32_t);

  auto transferBuffer = m_TransferManager->getTransferPool().acquireUnique(size);
  transferBuffer->copy(ptr, size, 0);
  m_TransferManager->addTask(std::move(transferBuffer), m_Buffer, 0, 0, size);
}

vk::Buffer VulkanIndexBuffer::getBuffer() const {
  return m_Buffer;
}

}   // namespace engine::vulkan::render::buffer