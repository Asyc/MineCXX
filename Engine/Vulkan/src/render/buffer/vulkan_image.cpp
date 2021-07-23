#include "engine/vulkan/render/buffer/vulkan_image.hpp"

#include <stb_image.h>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "engine/vulkan/vulkan_context.hpp"

namespace engine::vulkan::render::buffer {

VulkanImage::VulkanImage(VulkanTransferManager* transferManager, VmaAllocator allocator, const File& path) {
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    auto* buffer = stbi_load(path.getFullPath().c_str(), &width, &height, &channels, STBI_rgb_alpha);
    size_t length = width * height * 4;

    if (buffer == nullptr) {
        throw std::runtime_error("failed to load image");
    }

    m_Width = width;
    m_Height = height;

    VkImageCreateInfo imageCreateInfo = vk::ImageCreateInfo(
        {},
        vk::ImageType::e2D,
        vk::Format::eR8G8B8A8Srgb,
        {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1},
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

    m_Image = image;
    m_Allocation = {allocation, [allocator, image](VmaAllocation allocation){
        vmaDestroyImage(allocator, image, allocation);
    }};

    auto transferBuffer = transferManager->getTransferPool().acquireUnique(length);
    transferBuffer->copy(buffer, length, 0);
    stbi_image_free(buffer);

    vk::BufferImageCopy copy(
        0,
        0,
        0,
        vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
        {},
        imageCreateInfo.extent
    );

    transferManager->addTaskImage(std::move(transferBuffer), m_Image, copy);

    VmaAllocatorInfo allocatorInfo;
    vmaGetAllocatorInfo(allocator, &allocatorInfo);

    vk::Device device = allocatorInfo.device;

    vk::ImageViewCreateInfo imageViewCreateInfo({}, m_Image, vk::ImageViewType::e2D, vk::Format::eR8G8B8A8Srgb, {}, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
    m_ImageView = device.createImageViewUnique(imageViewCreateInfo);

    vk::SamplerCreateInfo samplerCreateInfo(
        {},
        vk::Filter::eNearest,
        vk::Filter::eNearest,
        vk::SamplerMipmapMode::eNearest,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat
    );

    m_Sampler = device.createSamplerUnique(samplerCreateInfo);
}

}   // namespace engine::vulkan::render::buffer