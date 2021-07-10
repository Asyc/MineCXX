#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_INDEX_BUFFER_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_INDEX_BUFFER_HPP_

#include "engine/render/buffer/index_buffer.hpp"

#include <vulkan/vulkan.hpp>

#include "engine/vulkan/render/buffer/vulkan_transfer_pool.hpp"

namespace engine::render::vulkan {
class VulkanRenderContext;
}

namespace engine::render::buffer::vulkan {

class VulkanIndexBuffer : public IndexBuffer {
public:
    VulkanIndexBuffer(vk::Device device, uint32_t memoryTypeIndex, size_t size, engine::render::vulkan::VulkanRenderContext* context, VulkanTransferPool* transferPool);

    void write(size_t offset, const uint32_t* ptr, size_t length) override;

    [[nodiscard]] vk::Buffer getBuffer() const;
private:
    engine::render::vulkan::VulkanRenderContext* m_Context;

    vk::UniqueBuffer m_Buffer;
    vk::UniqueDeviceMemory m_Allocation;

    VulkanTransferPool* m_TransferPool;
};

}   // namespace engine::render::buffer::vulkan




#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_INDEX_BUFFER_HPP_
