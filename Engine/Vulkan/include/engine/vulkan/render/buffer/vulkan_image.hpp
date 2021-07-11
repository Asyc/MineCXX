#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_VULKAN_IMAGE_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_VULKAN_IMAGE_HPP_

#include "engine/render/buffer/image.hpp"

#include <vulkan/vulkan.hpp>

#include "engine/file.hpp"
#include "engine/vulkan/render/buffer/vulkan_transfer_pool.hpp"

namespace engine::render::vulkan {
class VulkanRenderContext;
}   // namespace engine::render::vulkan

namespace engine::render::buffer::vulkan {

class VulkanImage : public Image {
public:
    VulkanImage(vk::Device device, uint32_t memoryTypeIndex, engine::render::vulkan::VulkanRenderContext* context, VulkanTransferPool* transferPool, const File& path);
private:
    vk::UniqueImage m_Image;
    vk::UniqueDeviceMemory m_Allocation;

    vk::UniqueImageView m_ImageView;
};

}   // namespace engine::render::vulkan

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_VULKAN_IMAGE_HPP_
