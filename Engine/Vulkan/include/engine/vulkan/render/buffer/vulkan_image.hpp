#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_VULKAN_IMAGE_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_VULKAN_IMAGE_HPP_

#include "engine/render/buffer/image.hpp"

#include <vulkan/vulkan.hpp>

#include "engine/file.hpp"
#include "engine/vulkan/render/buffer/vulkan_transfer_pool.hpp"

#include "engine/vulkan/render/vulkan_descriptor.hpp"

namespace engine::vulkan::render::buffer {

using namespace ::engine::render::buffer;

class VulkanImage : public Image, public IVulkanDescriptorResource {
 public:
  VulkanImage(VulkanTransferManager* transferManager, VmaAllocator allocator, const File& path, const SamplerOptions& samplerOptions);
  VulkanImage(VulkanTransferManager* transferManager, VmaAllocator allocator, void* buffer, uint32_t width, uint32_t height, const SamplerOptions& samplerOptions);

  [[nodiscard]] size_t getWidth() const override { return m_Width; }
  [[nodiscard]] size_t getHeight() const override { return m_Height; }

  [[nodiscard]] vk::DescriptorType getDescriptorType() const override { return vk::DescriptorType::eCombinedImageSampler; }
  [[nodiscard]] vk::Sampler getSampler() const override { return *m_Sampler; }
  [[nodiscard]] vk::ImageView getImageView() const override { return *m_ImageView; }
 private:
  friend void createImage(VmaAllocator allocator, VulkanTransferManager* transferManager, VulkanImage* handle, const void* buffer, const SamplerOptions& samplerOptions);

  vk::Image m_Image;
  size_t m_Width, m_Height;

  std::unique_ptr<VmaAllocation_T, std::function<void(VmaAllocation)>> m_Allocation;
  vk::UniqueDescriptorSet m_DescriptorSet;

  vk::UniqueImageView m_ImageView;
  vk::UniqueSampler m_Sampler;
};

}   // namespace engine::render::vulkan

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_VULKAN_IMAGE_HPP_
