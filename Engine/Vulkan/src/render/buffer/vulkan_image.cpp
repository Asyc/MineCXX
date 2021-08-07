#include "engine/vulkan/render/buffer/vulkan_image.hpp"

#include <stb_image.h>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "engine/vulkan/vulkan_context.hpp"

namespace engine::vulkan::render::buffer {

using namespace ::engine::render::buffer;

vk::SamplerAddressMode toAddressMode(engine::render::buffer::Image::RepeatMode mode) {
  switch (mode) {
    case engine::render::buffer::Image::RepeatMode::REPEAT: return vk::SamplerAddressMode::eRepeat;
    case engine::render::buffer::Image::RepeatMode::CLAMP_TO_BORDER: return vk::SamplerAddressMode::eClampToBorder;
    case engine::render::buffer::Image::RepeatMode::CLAMP_TO_EDGE: return vk::SamplerAddressMode::eClampToEdge;
    default:throw std::runtime_error("enum not found");
  }
}

VulkanImage::VulkanImage(VulkanTransferManager* transferManager, VmaAllocator allocator, const File& path, const SamplerOptions& samplerOptions) {
  int width, height, channels;

  auto* buffer = stbi_load(path.getPath().c_str(), &width, &height, &channels, STBI_rgb_alpha);
  if (buffer == nullptr) {
    throw std::runtime_error("failed to load image");
  }

  m_Width = width;
  m_Height = height;
  createImage(allocator, transferManager, this, buffer, samplerOptions);
  stbi_image_free(buffer);
}

VulkanImage::VulkanImage(VulkanTransferManager* transferManager, VmaAllocator allocator, void* buffer, uint32_t width, uint32_t height, const Image::SamplerOptions& samplerOptions) {
  m_Width = width;
  m_Height = height;
  createImage(allocator, transferManager, this, buffer, samplerOptions);
}

void createImage(VmaAllocator allocator, VulkanTransferManager* transferManager, VulkanImage* handle, const void* buffer, const VulkanImage::SamplerOptions& samplerOptions) {
  size_t length = handle->m_Width * handle->m_Height * 4;

  VkImageCreateInfo imageCreateInfo = vk::ImageCreateInfo(
      {},
      vk::ImageType::e2D,
      vk::Format::eR8G8B8A8Srgb,
      {static_cast<uint32_t>(handle->m_Width), static_cast<uint32_t>(handle->m_Height), 1},
      1,
      1,
      vk::SampleCountFlagBits::e1,
      vk::ImageTiling::eLinear,
      vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
      vk::SharingMode::eExclusive,
      {},
      vk::ImageLayout::eUndefined
  );

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  VkImage image;
  VmaAllocation allocation;
  VmaAllocationInfo allocationInfo;
  vmaCreateImage(allocator, &imageCreateInfo, &allocationCreateInfo, &image, &allocation, &allocationInfo);

  handle->m_Image = image;
  handle->m_Allocation = {allocation, [allocator, image](VmaAllocation allocation) {
    vmaDestroyImage(allocator, image, allocation);
  }};

  auto transferBuffer = transferManager->getTransferPool().acquireUnique(length);
  transferBuffer->copy(buffer, length, 0);

  vk::BufferImageCopy copy(
      0,
      0,
      0,
      vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
      {},
      imageCreateInfo.extent
  );

  transferManager->addTaskImage(std::move(transferBuffer), handle->m_Image, copy);

  VmaAllocatorInfo allocatorInfo;
  vmaGetAllocatorInfo(allocator, &allocatorInfo);

  vk::Device device = allocatorInfo.device;

  vk::ImageViewCreateInfo imageViewCreateInfo({}, handle->m_Image, vk::ImageViewType::e2D, vk::Format::eR8G8B8A8Srgb, {}, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
  handle->m_ImageView = device.createImageViewUnique(imageViewCreateInfo);

  vk::SamplerCreateInfo samplerCreateInfo(
      {},
      samplerOptions.minFilter == VulkanImage::Filter::NEAREST ? vk::Filter::eNearest : vk::Filter::eLinear,
      samplerOptions.magFilter == VulkanImage::Filter::NEAREST ? vk::Filter::eNearest : vk::Filter::eLinear,
      vk::SamplerMipmapMode::eNearest,
      toAddressMode(samplerOptions.repeatX),
      toAddressMode(samplerOptions.repeatY)
  );

  handle->m_Sampler = device.createSamplerUnique(samplerCreateInfo);
}

}   // namespace engine::vulkan::render::buffer