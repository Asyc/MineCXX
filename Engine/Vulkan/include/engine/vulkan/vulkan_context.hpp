#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_VULKAN_CONTEXT_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_VULKAN_CONTEXT_HPP_

#include "engine/render/render_context.hpp"

#include <memory>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "engine/vulkan/render/buffer/vulkan_transfer_pool.hpp"
#include "engine/vulkan/render/buffer/vulkan_uniform_buffer.hpp"
#include "engine/vulkan/render/buffer/vulkan_vertex_buffer.hpp"
#include "engine/vulkan/render/command/vulkan_command_pool.hpp"
#include "engine/vulkan/render/swapchain/vulkan_swapchain.hpp"
#include "engine/vulkan/device/vulkan_queue.hpp"
#include "engine/vulkan/device/vulkan_device.hpp"
#include "engine/window.hpp"

#include "engine/render/viewport.hpp"

namespace engine::vulkan::render {

using namespace ::engine::render;

class VulkanRenderContext : public RenderContext {
 public:
  VulkanRenderContext(Window& window, const Directory& resourceDirectory, Swapchain::SwapchainMode modeHint);
  ~VulkanRenderContext() override;

  std::shared_ptr<buffer::Image> createImage(const File& path, const buffer::Image::SamplerOptions& samplerOptions) override;
  std::shared_ptr<buffer::Image> createImage(void* buffer, uint32_t width, uint32_t height, const buffer::Image::SamplerOptions& samplerOptions) override;

  [[nodiscard]] std::unique_ptr<buffer::VertexBuffer> allocateVertexBuffer(size_t size) override;
  [[nodiscard]] std::unique_ptr<buffer::IndexBuffer> allocateIndexBuffer(size_t size) override;
  [[nodiscard]] std::unique_ptr<buffer::UniformBuffer> allocateUniformBuffer(size_t size) override;
  [[nodiscard]] std::unique_ptr<buffer::StorageBuffer> allocateStorageBuffer(size_t size) override;

  [[nodiscard]] std::shared_ptr<RenderPipeline> createRenderPipeline(const File& file) override;

  [[nodiscard]] std::unique_ptr<command::CommandPool> createCommandPool() const override;
  [[nodiscard]] command::CommandPool& getThreadCommandPool() override;

  [[nodiscard]] Swapchain& getSwapchain() override { return m_Swapchain; }
  [[nodiscard]] const Swapchain& getSwapchain() const override { return m_Swapchain; }

  [[nodiscard]] VulkanSwapchain& getSwapchainVulkan() { return m_Swapchain; }
  [[nodiscard]] device::VulkanDevice& getDevice() { return m_Device; }

  [[nodiscard]] const ViewportGUI& getViewport() const override { return m_GuiViewport; }

  [[nodiscard]] Camera& getCamera() override { return m_Camera; }
  [[nodiscard]] const Camera& getCamera() const override { return m_Camera; }
  [[nodiscard]] gui::font::FontRenderer& getFontRenderer() override { return m_FontRenderer; }

  [[nodiscard]] Window& getWindow() override { return *m_Window; }
  [[nodiscard]] const Window& getWindow() const override { return *m_Window; }

  void addResizeCallback(ResizeCallback callback) override;
  void addMouseCallback(MouseCallback callback) override;
  void addMousePositionCallback(MousePositionCallback callback) override;

  void mouseButtonCallback(gui::input::MouseButton button, gui::input::MouseButtonAction action, double x, double y) override;
  void mousePositionCallback(double x, double y) override;
  void windowResizeCallback(uint32_t width, uint32_t height) override;
 private:
  vk::UniqueInstance m_Instance;
#ifdef MCE_DBG
  std::unique_ptr<VkDebugUtilsMessengerEXT_T, std::function<void(VkDebugUtilsMessengerEXT)>> m_DebugMessenger;
  std::unique_ptr<VkDebugReportCallbackEXT_T, std::function<void(VkDebugReportCallbackEXT)>> m_DebugReportCallback;
#endif
  Window* m_Window;

  vk::UniqueSurfaceKHR m_Surface;
  vk::PhysicalDevice m_PhysicalDevice;

  device::VulkanDevice m_Device;

  render::VulkanSwapchain m_Swapchain;

  std::unordered_map<std::string, std::weak_ptr<VulkanRenderPipeline>> m_Pipelines;

  std::vector<ResizeCallback> m_ResizeCallbacks;
  std::vector<MouseCallback> m_MouseCallbacks;
  std::vector<MousePositionCallback> m_MousePositionCallbacks;

  ViewportGUI m_GuiViewport;
  Camera m_Camera;
  gui::font::FontRenderer m_FontRenderer;

  static thread_local std::unordered_map<VulkanRenderContext*, command::VulkanCommandPool> s_ThreadCommandPoolMap;
};

}   // namespace engine::render::vulkan

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_VULKAN_CONTEXT_HPP_
