#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_VULKAN_DEVICE_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_VULKAN_DEVICE_HPP_

#include <array>
#include <optional>

#include <vulkan/vulkan.hpp>

#include "vulkan_queue.hpp"
#include "engine/vulkan/render/buffer/vulkan_transfer_pool.hpp"

namespace engine::vulkan::render {
class VulkanRenderContext;
}   // namespace engine::vulkan::render

namespace engine::vulkan::device {

using namespace engine;

class VulkanDevice {
 public:
  VulkanDevice(render::VulkanRenderContext* context, vk::Instance owner, vk::PhysicalDevice device, vk::SurfaceKHR surface);

  [[nodiscard]] vk::Device getDevice() const {
    return *m_Device;
  }

  VulkanQueueManager& getQueueManager() { return m_QueueManager; }
  [[nodiscard]] const VulkanQueueManager& getQueueManager() const { return m_QueueManager; };

  [[nodiscard]] VmaAllocator getAllocator() const { return m_MemoryAllocator.get(); }
  [[nodiscard]] render::buffer::VulkanTransferManager& getTransferManager() { return m_TransferManager; }
 private:
  vk::UniqueDevice m_Device;
  VulkanQueueManager m_QueueManager;

  std::unique_ptr<VmaAllocator_T, std::function<void(VmaAllocator)>> m_MemoryAllocator;
  render::buffer::VulkanTransferManager m_TransferManager;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_VULKAN_DEVICE_HPP_
