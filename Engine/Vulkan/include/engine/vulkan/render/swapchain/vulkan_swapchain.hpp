#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_SWAPCHAIN_VULKAN_SWAPCHAIN_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_SWAPCHAIN_VULKAN_SWAPCHAIN_HPP_

#include "engine/render/swapchain.hpp"

#include <atomic>
#include <deque>
#include <mutex>
#include <vector>

#include "engine/vulkan/render/buffer/vulkan_image.hpp"
#include "engine/vulkan/device/vulkan_device.hpp"
#include "engine/vulkan/device/vulkan_queue.hpp"
#include "vulkan_image.hpp"

namespace engine::vulkan::render {

using namespace ::engine::render;
using namespace ::engine::render::command;

class VulkanSwapchain : public Swapchain {
 public:
  VulkanSwapchain(Swapchain::SwapchainMode modeHint,
                  vk::SurfaceKHR surface,
                  vk::PhysicalDevice physicalDevice,
                  device::VulkanDevice* device,
                  VmaAllocator allocator);
  ~VulkanSwapchain() override;

  void addResourceFree(std::shared_ptr<void> resource) override;

  void nextImage() override;

  void submitCommandBuffer(const DirectCommandBuffer& buffer) override;
  void submitCommandBuffer(const IndirectCommandBuffer& buffer) override;

  void onResize(uint32_t width, uint32_t height) override;

  [[nodiscard]] SwapchainMode getSwapchainMode() const override;
  [[nodiscard]] const std::set<SwapchainMode>& getSupportedSwapchainModes() const override;

  [[nodiscard]] uint32_t getCurrentFrameIndex() const override;
  [[nodiscard]] uint32_t getFrameCount() const override;

  [[nodiscard]] std::pair<uint32_t, uint32_t> getSize() const override;

  [[nodiscard]] vk::RenderPass getRenderPass() const;
  [[nodiscard]] vk::Framebuffer getCurrentFrame() const;

  [[nodiscard]] vk::Extent2D getExtent() const;

  [[nodiscard]] vk::Format getDepthBufferFormat() const { return m_DepthBufferFormat; }

 private:
  struct ResourceFreeFlight {
    std::deque<std::shared_ptr<void>> resources;
  };
  void createSwapchain();
  void setupImage();

  SwapchainMode m_Mode;
  std::set<SwapchainMode> m_SupportedModes;

  device::VulkanDevice* m_Owner;
  VmaAllocator m_Allocator;
  vk::SurfaceKHR m_Surface;
  vk::Queue m_GraphicsQueue;

  vk::PresentModeKHR m_PresentMode;
  vk::SurfaceFormatKHR m_SwapchainFormat;
  vk::Extent2D m_SwapchainExtent;
  uint32_t m_SwapchainImageCountMin;

  vk::Format m_DepthBufferFormat;

  vk::UniqueRenderPass m_RenderPass;
  vk::UniqueCommandPool m_CommandPool;

  vk::UniqueSwapchainKHR m_Swapchain;

  std::vector<Image> m_SwapchainImages;
  std::vector<ImageFlight> m_RenderFlights;
  size_t m_CurrentFlight;

  std::vector<ResourceFreeFlight> m_ResourceFlights;
};

}   // namespace engine::vulkan::render


#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_SWAPCHAIN_VULKAN_SWAPCHAIN_HPP_
