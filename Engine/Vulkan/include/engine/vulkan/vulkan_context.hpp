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

namespace engine::vulkan::render {

using namespace ::engine::render;

class VulkanRenderContext : public RenderContext {
public:
    VulkanRenderContext(const Window& window, const Directory& resourceDirectory, Swapchain::SwapchainMode modeHint);

    std::unique_ptr<buffer::Image> createImage(const File& path) override;

    [[nodiscard]] std::unique_ptr<buffer::VertexBuffer> allocateVertexBuffer(size_t size) override;
    [[nodiscard]] std::unique_ptr<buffer::IndexBuffer> allocateIndexBuffer(size_t size) override;
    [[nodiscard]] std::unique_ptr<buffer::UniformBuffer> allocateUniformBuffer(const RenderPipeline& pipeline, size_t size) override;

    [[nodiscard]] std::unique_ptr<RenderPipeline> createRenderPipeline(const File& file) const override;

    [[nodiscard]] std::unique_ptr<command::CommandPool> createCommandPool() const override;
    [[nodiscard]] command::CommandPool& getThreadCommandPool() override;

    [[nodiscard]] Swapchain& getSwapchain() override { return m_Swapchain; }
    [[nodiscard]] const Swapchain& getSwapchain() const override { return m_Swapchain; }

    void mouseButtonCallback(gui::input::MouseButton button, gui::input::MouseButtonAction action, double x, double y) override;

    [[nodiscard]] VulkanSwapchain& getSwapchainVulkan() { return m_Swapchain; }
    [[nodiscard]] device::VulkanDevice& getDevice() { return m_Device; }

    [[nodiscard]] buffer::VulkanTransferManager& getTransferManager() { return m_TransferManager; }

    [[nodiscard]] gui::font::FontRenderer& getFontRenderer() override { return m_FontRenderer; }

    void setResizeCallback(ResizeCallback callback) override { m_ResizeCallback = std::move(callback); }

    [[nodiscard]] const ResizeCallback& getResizeCallback() { return m_ResizeCallback; }
private:
    vk::UniqueInstance m_Instance;
#ifdef MCE_DBG
    std::unique_ptr<VkDebugUtilsMessengerEXT_T, std::function<void(VkDebugUtilsMessengerEXT)>> m_DebugMessenger;
    std::unique_ptr<VkDebugReportCallbackEXT_T, std::function<void(VkDebugReportCallbackEXT)>> m_DebugReportCallback;
#endif

    vk::UniqueSurfaceKHR m_Surface;
    vk::PhysicalDevice m_PhysicalDevice;

    device::VulkanDevice m_Device;
    std::unique_ptr<VmaAllocator_T, std::function<void(VmaAllocator)>> m_MemoryAllocator;

    render::VulkanSwapchain m_Swapchain;
    buffer::VulkanTransferManager m_TransferManager;

    vk::UniqueDescriptorPool m_DescriptorPool;

    gui::font::FontRenderer m_FontRenderer;

    ResizeCallback m_ResizeCallback;

    static thread_local std::unordered_map<VulkanRenderContext*, command::VulkanCommandPool> s_ThreadCommandPoolMap;
};

}   // namespace engine::render::vulkan

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_VULKAN_CONTEXT_HPP_
