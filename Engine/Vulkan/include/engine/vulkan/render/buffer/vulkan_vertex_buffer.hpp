#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_VERTEX_BUFFER_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_VERTEX_BUFFER_HPP_

#include "engine/render/buffer/vertex_buffer.hpp"

#include <vulkan/vulkan.hpp>

#include "engine/vulkan/render/buffer/vulkan_transfer_pool.hpp"

namespace engine::vulkan::render::buffer {

using namespace ::engine::render::buffer;

class VulkanVertexBuffer : public VertexBuffer {
 public:
  VulkanVertexBuffer(VulkanTransferManager* transferManager, VmaAllocator allocator, size_t size);

  void write(size_t offset, const void* ptr, size_t length) override;

  [[nodiscard]] vk::Buffer getBuffer() const {
    return m_Buffer;
  }
 private:
  vk::Buffer m_Buffer;
  std::unique_ptr<VmaAllocation_T, std::function<void(VmaAllocation)>> m_Allocation;

  VulkanTransferManager* m_TransferManager;
};

}   // namespace engine::render::buffer::vulkan

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_VERTEX_BUFFER_HPP_
