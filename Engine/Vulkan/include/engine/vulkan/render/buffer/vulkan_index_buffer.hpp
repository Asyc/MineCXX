#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_INDEX_BUFFER_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_INDEX_BUFFER_HPP_

#include "engine/render/buffer/index_buffer.hpp"

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

#include "engine/vulkan/render/buffer/vulkan_transfer_pool.hpp"

#include "engine/vulkan/device/vulkan_device.hpp"

namespace engine::vulkan::render::buffer {

using namespace ::engine::render::buffer;

class VulkanIndexBuffer : public IndexBuffer {
public:
    VulkanIndexBuffer(VulkanTransferManager* transferManager, VmaAllocator allocator, size_t size);

    void write(size_t offset, const uint32_t* ptr, size_t length) override;

    [[nodiscard]] vk::Buffer getBuffer() const;
private:
    vk::Buffer m_Buffer;
    std::unique_ptr<VmaAllocation_T, std::function<void(VmaAllocation)>> m_Allocation;
    VmaAllocationInfo m_AllocationInfo;

    VulkanTransferManager* m_TransferManager;
};

}   // namespace engine::vulkan::render::buffer




#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_INDEX_BUFFER_HPP_
