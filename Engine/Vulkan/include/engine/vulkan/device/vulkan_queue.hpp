#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_VULKAN_QUEUE_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_VULKAN_QUEUE_HPP_

#include <cstdint>
#include <optional>

#include <vulkan/vulkan.hpp>

namespace engine::vulkan::render {
class VulkanRenderContext;
}

namespace engine::vulkan::device {

struct VulkanQueueFamily {
  uint32_t index;
  uint32_t queueCount;
};

class VulkanDevice;

class VulkanQueueManager {
 public:
  VulkanQueueManager(render::VulkanRenderContext* context, const VulkanQueueFamily& graphics, const VulkanQueueFamily& present);

  void submitTransfer(uint32_t count, const vk::SubmitInfo* submitInfo, vk::Fence fence);
  void submitGraphics(uint32_t count, const vk::SubmitInfo* submitInfo, vk::Fence fence);

  [[nodiscard]] const VulkanQueueFamily& getGraphicsQueueFamily() const { return m_GraphicsFamily; }

  [[nodiscard]]vk::Queue getGraphicsQueue() const { return m_GraphicsQueue; }
  [[nodiscard]]vk::Queue getTransferQueue() const { return m_TransferQueue; }
  [[nodiscard]]vk::Queue getPresentQueue() const { return m_PresentQueue; }
 private:
  render::VulkanRenderContext* m_Context;

  VulkanQueueFamily m_GraphicsFamily;
  VulkanQueueFamily m_PresentFamily;

  vk::Queue m_TransferQueue;
  vk::Queue m_GraphicsQueue;
  vk::Queue m_PresentQueue;

  vk::Semaphore m_Semaphore;

  std::vector<vk::Semaphore> m_TransferSemaphores;
  std::vector<vk::Semaphore> m_ReleaseSemaphores;
};

}   // namespace engine::vulkan::device

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_VULKAN_QUEUE_HPP_
