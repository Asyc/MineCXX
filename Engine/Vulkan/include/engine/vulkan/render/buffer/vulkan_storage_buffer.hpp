#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_SSBO_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_SSBO_HPP_

#include "engine/render/buffer/uniform_buffer.hpp"

#include <optional>

#include <vulkan/vulkan.hpp>

#include "engine/render/buffer/storage_buffer.hpp"
#include "engine/vulkan/render/buffer/vulkan_transfer_pool.hpp"
#include "engine/vulkan/render/vulkan_descriptor.hpp"
#include "engine/vulkan/render/vulkan_pipeline.hpp"

namespace engine::vulkan::render::buffer {

using namespace ::engine::render::buffer;

class VulkanStorageBuffer : public StorageBuffer, public IVulkanDescriptorResource {
 public:
  VulkanStorageBuffer(VmaAllocator allocator, size_t size);

  void write(size_t offset, const void* ptr, size_t length) override;

  [[nodiscard]] vk::DescriptorType getDescriptorType() const override { return vk::DescriptorType::eStorageBuffer; }
  [[nodiscard]] vk::Buffer getBuffer() const override { return m_Buffer; }
 protected:
  vk::Buffer m_Buffer;
  std::unique_ptr<VmaAllocation_T, std::function<void(VmaAllocation)>> m_Allocation;
  vk::UniqueDescriptorSet m_DescriptorSet;

  void* m_MappedPtr;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_SSBO_HPP_
