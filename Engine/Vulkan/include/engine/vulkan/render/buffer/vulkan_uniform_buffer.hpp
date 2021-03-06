#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_UNIFORM_BUFFER_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_UNIFORM_BUFFER_HPP_

#include "engine/render/buffer/uniform_buffer.hpp"

#include <optional>

#include <vulkan/vulkan.hpp>

#include "engine/vulkan/render/buffer/vulkan_transfer_pool.hpp"
#include "engine/vulkan/render/vulkan_descriptor.hpp"
#include "engine/vulkan/render/vulkan_pipeline.hpp"

VK_DEFINE_HANDLE(VmaAllocation)

namespace engine::vulkan::render {
class VulkanSwapchain;
}

namespace engine::vulkan::render::buffer {

using namespace ::engine::render::buffer;

class VulkanUniformBuffer : public UniformBuffer, public IVulkanDescriptorResource {
 public:
  VulkanUniformBuffer(VmaAllocator allocator, size_t size);

  void write(size_t offset, const void* ptr, size_t length) override;

  [[nodiscard]] vk::DescriptorType getDescriptorType() const override { return vk::DescriptorType::eUniformBuffer; }
  [[nodiscard]] vk::Buffer getBuffer() const override { return m_Buffer; }
 protected:
  vk::Buffer m_Buffer;
  std::unique_ptr<VmaAllocation_T, std::function<void(VmaAllocation)>> m_Allocation;
  vk::UniqueDescriptorSet m_DescriptorSet;

  void* m_MappedPtr;
};

class VulkanSwitchingUniformBuffer : public UniformBuffer, public IVulkanDescriptorResource {
 public:
  VulkanSwitchingUniformBuffer(VmaAllocator allocator, size_t size);

  void write(size_t offset, const void* ptr, size_t length) override;

  [[nodiscard]] vk::DescriptorType getDescriptorType() const override { return vk::DescriptorType::eUniformBuffer; }
  [[nodiscard]] vk::Buffer getBuffer() const override;
 protected:
  VulkanSwapchain* m_Swapchain;

  struct Buffer {
    vk::Buffer buffer;
    std::unique_ptr<VmaAllocation_T, std::function<void(VmaAllocation)>> m_Allocation;
    vk::UniqueDescriptorSet m_DescriptorSet;
    void* m_MappedPtr;
  };

  std::vector<Buffer> m_Stages;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_UNIFORM_BUFFER_HPP_
