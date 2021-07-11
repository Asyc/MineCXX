#include "engine/vulkan/render/buffer/vulkan_image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "engine/vulkan/vulkan_context.hpp"

namespace engine::render::buffer::vulkan {

VulkanImage::VulkanImage(vk::Device device, uint32_t memoryTypeIndex, engine::render::vulkan::VulkanRenderContext* context, VulkanTransferPool* transferPool, const File& path) {
    int width, height, channels;
    auto* buffer = stbi_load(path.getFullPath().c_str(), &width, &height, &channels, STBI_rgb_alpha);
    size_t length = width * height * 4;

    if (buffer == nullptr) {
        throw std::runtime_error("failed to load image");
    }

    vk::ImageCreateInfo imageCreateInfo(
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
    m_Image = device.createImageUnique(imageCreateInfo);

    vk::MemoryRequirements memoryRequirements = device.getImageMemoryRequirements(*m_Image);

    vk::MemoryAllocateInfo allocateInfo(memoryRequirements.size, memoryTypeIndex);
    m_Allocation = device.allocateMemoryUnique(allocateInfo);

    device.bindImageMemory(*m_Image, *m_Allocation, 0);

    auto transferBuffer = transferPool->acquireUnique(length);
    transferBuffer->copy(buffer, length, 0);

    vk::BufferImageCopy copy(
        0,
        0,
        0,
        vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
        {},
        imageCreateInfo.extent
    );

    context->getSwapchainVulkan().addTransferImage(std::move(transferBuffer), *m_Image, copy);

    stbi_image_free(buffer);

    vk::ImageViewCreateInfo imageViewCreateInfo({}, *m_Image, vk::ImageViewType::e2D, imageCreateInfo.format, {}, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
    m_ImageView = device.createImageViewUnique(imageViewCreateInfo);
}

}   // namespace namespace engine::render::buffer::vulkan