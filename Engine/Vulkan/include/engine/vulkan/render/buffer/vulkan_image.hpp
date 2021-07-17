#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_VULKAN_IMAGE_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_VULKAN_IMAGE_HPP_

#include "engine/render/buffer/image.hpp"

#include <vulkan/vulkan.hpp>

#include "engine/file.hpp"
#include "engine/vulkan/render/buffer/vulkan_transfer_pool.hpp"

namespace engine::vulkan::render::buffer {

using namespace ::engine::render::buffer;

class VulkanImage : public Image {
public:
    VulkanImage(VulkanTransferManager* transferManager, VmaAllocator allocator, const File& path);
private:
    vk::Image m_Image;
    std::unique_ptr<VmaAllocation_T, std::function<void(VmaAllocation)>> m_Allocation;

    VulkanTransferManager* m_TransferManager;

};

}   // namespace engine::render::vulkan

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_VULKAN_IMAGE_HPP_
