#include "engine/vulkan/device/vulkan_queue.hpp"

#include "engine/vulkan/vulkan_context.hpp"

namespace engine::vulkan::device {

VulkanQueueManager::VulkanQueueManager(render::VulkanRenderContext* context, const VulkanQueueFamily& graphics, const VulkanQueueFamily& present)
    : m_Context(context),
      m_GraphicsFamily(graphics),
      m_PresentFamily(present),
      m_TransferQueue(m_Context->getDevice().getDevice().getQueue(graphics.index, 0)),
      m_GraphicsQueue(m_Context->getDevice().getDevice().getQueue(graphics.index, graphics.queueCount > 1 ? 1 : 0)),
      m_PresentQueue(m_Context->getDevice().getDevice().getQueue(present.index, 0)) {}

void VulkanQueueManager::submitTransfer(uint32_t count, const vk::SubmitInfo* submitInfo, vk::Fence fence) {
  m_TransferQueue.submit(count, submitInfo, fence);
}

void VulkanQueueManager::submitGraphics(uint32_t count, const vk::SubmitInfo* submitInfo, vk::Fence fence) {
  m_Context->getTransferManager().submit(false);
  m_TransferQueue.waitIdle(); //todo : revise
  m_GraphicsQueue.submit(count, submitInfo, fence);
}

}   // namespace engine::vulkan::device
