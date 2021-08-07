#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_VULKAN_SWAPCHAIN2_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_VULKAN_SWAPCHAIN2_HPP_

#include <functional>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "engine/vulkan/device/vulkan_device.hpp"

namespace engine::vulkan::render {

struct Image {
  Image(device::VulkanDevice* device,
        vk::Image image,
        uint32_t imageIndex,
        vk::Format format,
        vk::Extent2D extent,
        vk::RenderPass renderPass,
        vk::CommandPool commandPool);
  Image(Image&& rhs) = default;
  ~Image();

  void wait();
  void setup(vk::Queue queue, vk::Semaphore imageAvailableSemaphore);
  void present(vk::Queue queue, vk::SwapchainKHR swapchain);

  vk::Semaphore getSemaphore();

  device::VulkanDevice* owner;

  uint32_t imageIndex;

  vk::UniqueImageView imageView;
  vk::UniqueFramebuffer framebuffer;

  std::vector<vk::Semaphore> semaphores;
  size_t activeSemaphoreCount;

  vk::UniqueCommandBuffer clearScreenCommandBuffer;

  vk::UniqueSemaphore clearScreenSemaphore;
  vk::UniqueFence clearScreenFence;

  vk::UniqueCommandBuffer presentFormatCommandBuffer;

  vk::UniqueSemaphore imageCompleteSemaphore;
  vk::UniqueFence imageCompleteFence;
};

struct ImageFlight {
  explicit ImageFlight(vk::Device device);
  void submitCommandBuffer(vk::Device device, vk::CommandBuffer buffer, vk::Fence fence) const;

  Image* boundImage;
  vk::UniqueSemaphore imageReadySemaphore;
};

}   // namespace engine::vulkan::render

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_VULKAN_SWAPCHAIN2_HPP_
