#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_VULKAN_CONTEXT_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_VULKAN_CONTEXT_HPP_

#include "engine/render/render_context.hpp"

#include <vector>

#include <vulkan/vulkan.hpp>

#include "engine/vulkan/render/buffer/vulkan_transfer_pool.hpp"
#include "engine/vulkan/render/buffer/vulkan_vertex_buffer.hpp"
#include "engine/vulkan/render/command/vulkan_command_pool.hpp"
#include "engine/vulkan/render/swapchain/vulkan_swapchain.hpp"
#include "engine/window.hpp"
#include "vulkan_queue.hpp"

namespace engine::render::vulkan {

class VulkanRenderContext : public render::RenderContext {
public:
    VulkanRenderContext(const Window& window, Swapchain::SwapchainMode modeHint);

    [[nodiscard]] std::unique_ptr<buffer::VertexBuffer> allocateVertexBuffer(size_t size) override;
    [[nodiscard]] std::unique_ptr<buffer::IndexBuffer> allocateIndexBuffer(size_t size) override;

    [[nodiscard]] std::unique_ptr<RenderPipeline> createRenderPipeline(const File& file) const override;

    [[nodiscard]] std::unique_ptr<command::CommandPool> createCommandPool() const override;

    [[nodiscard]] command::CommandPool& getThreadCommandPool() const;

    [[nodiscard]] Swapchain& getSwapchain() override;
    [[nodiscard]] const Swapchain& getSwapchain() const override;

    [[nodiscard]] VulkanSwapchain& getSwapchainVulkan();
private:
    vk::UniqueInstance m_Instance;
    vk::UniqueSurfaceKHR m_Surface;
    vk::PhysicalDevice m_PhysicalDevice;
    vk::UniqueDevice m_Device;

    queue::QueueFamilyTable m_QueueTable;

    std::unique_ptr<VulkanSwapchain> m_Swapchain;

    uint32_t m_TransferMemoryTypeIndex, m_LocalMemoryTypeIndex;

    std::unique_ptr<buffer::vulkan::VulkanTransferPool> m_TransferPool;

    static thread_local std::unordered_map<const VulkanRenderContext*, std::unique_ptr<command::vulkan::VulkanCommandPool>> s_ThreadCommandPoolMap;
};

}   // namespace engine::render::vulkan

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_VULKAN_CONTEXT_HPP_
