#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_TRANSFER_POOL_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_TRANSFER_POOL_HPP_

#include <cstdint>
#include <deque>
#include <map>
#include <mutex>
#include <vector>

#include <vulkan/vulkan.hpp>

namespace engine::render::buffer::vulkan {

class VulkanTransferPool;

class VulkanTransferBuffer {
public:
    VulkanTransferBuffer() = default;
    VulkanTransferBuffer(vk::Device device, uint32_t memoryTypeIndex, size_t size);
    VulkanTransferBuffer(VulkanTransferBuffer&& rhs) = default;
    ~VulkanTransferBuffer();

    void copy(const void* src, size_t length, size_t offset) const;
    void write(vk::CommandBuffer buffer, size_t dstOffset, vk::Buffer dst, size_t length) const;

    [[nodiscard]] size_t getSize() const;
    [[nodiscard]] vk::Buffer getBuffer() const;
private:
    vk::UniqueBuffer m_Buffer;
    vk::UniqueDeviceMemory m_MemoryAlloc;

    size_t m_Size;
    void* m_MappedPtr;
};

class VulkanTransferBufferUnique;

class VulkanTransferPool {
public:
    VulkanTransferPool(vk::Device owner, uint32_t memoryTypeIndex);

    VulkanTransferBuffer* acquire(size_t minimumSize);
    void release(VulkanTransferBuffer* element);

    VulkanTransferBufferUnique acquireUnique(size_t minimumSize);
private:
    vk::Device m_Owner;
    uint32_t m_MemoryTypeIndex;

    std::vector<VulkanTransferBuffer> m_Buffers;
    std::deque<VulkanTransferBuffer*> m_Available;

    std::mutex m_Mutex;
};

class VulkanTransferBufferUnique {
public:
    VulkanTransferBufferUnique(VulkanTransferPool* owner, VulkanTransferBuffer* buffer);
    VulkanTransferBufferUnique(VulkanTransferBufferUnique&& rhs) noexcept;
    ~VulkanTransferBufferUnique();

    VulkanTransferBufferUnique& operator=(VulkanTransferBufferUnique&& rhs) noexcept;

    VulkanTransferBuffer& operator*() {
        return *m_Buffer;
    }

    const VulkanTransferBuffer& operator*() const {
        return *m_Buffer;
    }

    VulkanTransferBuffer* operator->() {
        return m_Buffer;
    }

    const VulkanTransferBuffer* operator->() const {
        return m_Buffer;
    }
private:
    VulkanTransferPool* m_Owner;
    VulkanTransferBuffer* m_Buffer;
};

}   // namespace engine::render::buffer::vulkan

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_TRANSFER_POOL_HPP_
