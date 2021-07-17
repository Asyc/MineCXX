#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_UNIFORM_BUFFER_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_UNIFORM_BUFFER_HPP_

#include "engine/render/buffer/uniform_buffer.hpp"

#include <optional>

#include <vulkan/vulkan.hpp>

#include "engine/vulkan/render/buffer/vulkan_transfer_pool.hpp"

VK_DEFINE_HANDLE(VmaAllocation)

namespace engine::vulkan::render::buffer {

using namespace ::engine::render::buffer;

class VulkanUniformBuffer : public UniformBuffer {
public:
    VulkanUniformBuffer(VmaAllocator allocator, size_t size);

    void write(size_t offset, const void* ptr, size_t length) override;
private:
    vk::Buffer m_Buffer;
    std::unique_ptr<VmaAllocation_T, std::function<void(VmaAllocation)>> m_Allocation;
};

class VulkanDirectUniformBuffer : public DirectUniformBuffer {
public:
    VulkanDirectUniformBuffer(VulkanTransferManager* transferManager, VmaAllocator allocator, size_t size);

    void write(size_t offset, const void* ptr, size_t length) override;
protected:
    std::optional<VulkanUniformBuffer> m_Backing;

    vk::Buffer m_Buffer;
    std::unique_ptr<VmaAllocation_T, std::function<void(VmaAllocation)>> m_Allocation;

    void* m_MappedPtr;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_UNIFORM_BUFFER_HPP_
