#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_TRANSFER_POOL_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_TRANSFER_POOL_HPP_

#include <atomic>
#include <cstdint>
#include <deque>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

#include <vulkan/vulkan.hpp>

VK_DEFINE_HANDLE(VmaAllocator)
VK_DEFINE_HANDLE(VmaAllocation)

namespace engine::vulkan {

namespace device {
class VulkanDevice;
}

namespace render::buffer {

class VulkanTransferBuffer {
 public:
  VulkanTransferBuffer() = default;
  VulkanTransferBuffer(VmaAllocator allocator, size_t size);

  void copy(const void* src, size_t length, size_t offset) const;

  [[nodiscard]] size_t getSize() const;
  [[nodiscard]] vk::Buffer getBuffer() const;
 private:
  vk::Buffer m_Buffer;
  std::unique_ptr<VmaAllocation_T, std::function<void(VmaAllocation)>> m_Allocation;

  void* m_MappedPtr;
  size_t m_Size;
};

class VulkanTransferBufferUnique;

class VulkanTransferPool {
 public:
  VulkanTransferPool(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device owner);

  VulkanTransferBuffer* acquire(size_t minimumSize);
  void release(VulkanTransferBuffer* element);

  VulkanTransferBufferUnique acquireUnique(size_t minimumSize);
 private:
  std::unique_ptr<VmaAllocator_T, std::function<void(VmaAllocator)>> m_Allocator;

  std::list<VulkanTransferBuffer> m_Buffers;
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
  VulkanTransferPool* m_Owner{};
  VulkanTransferBuffer* m_Buffer{};
};

class VulkanTransferManager {
 public:
  static constexpr size_t BACKING_COUNT = 4;

  VulkanTransferManager(vk::Instance instance,
                        vk::PhysicalDevice physicalDevice,
                        device::VulkanDevice* device,
                        uint32_t queueFamilyIndex);
  ~VulkanTransferManager();

  void submit(bool wait = false);

  void addTask(VulkanTransferBufferUnique src, vk::Buffer dst, size_t srcOffset, size_t dstOffset, size_t length);
  void addTaskImage(VulkanTransferBufferUnique src, vk::Image dst, vk::BufferImageCopy imageCopy);

  void setupDepthBufferImage(vk::Image dst);

  VulkanTransferPool& getTransferPool() {
    return m_TransferPool;
  }
 private:
  struct Flight {
    vk::UniqueCommandBuffer buffer;
    vk::UniqueFence fence;
    bool empty = true;
    std::vector<VulkanTransferBufferUnique> pendingRelease;
  };

  device::VulkanDevice* m_Device;

  vk::UniqueCommandPool m_CommandPool;

  std::deque<Flight> m_Flights;
  size_t m_FlightIndex;

  VulkanTransferPool m_TransferPool;
};

}   // namespace render::buffer
}   // namespace engine::vulkan


#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_TRANSFER_POOL_HPP_
