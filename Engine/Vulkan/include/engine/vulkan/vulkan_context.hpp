#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_VULKAN_CONTEXT_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_VULKAN_CONTEXT_HPP_

#include "engine/render/render_context.hpp"

#include <vulkan/vulkan.hpp>

#include "engine/vulkan/render/command/vulkan_command_pool.hpp"
#include "engine/vulkan/render/swapchain/vulkan_swapchain.hpp"
#include "engine/window.hpp"
#include "vulkan_queue.hpp"

namespace engine::render::vulkan {

class VulkanRenderContext : public render::RenderContext {
public:
    VulkanRenderContext(const Window& window, Swapchain::SwapchainMode modeHint);


    [[nodiscard]] std::unique_ptr<RenderPipeline> createRenderPipeline(const File& file) const override;

    [[nodiscard]] std::unique_ptr<command::CommandPool> createCommandPool() const override;

    [[nodiscard]] Swapchain& getSwapchain() override;
    [[nodiscard]] const Swapchain& getSwapchain() const override;

    vk::Instance getInstance() const;
    vk::PhysicalDevice getPhysicalDevice() const;
    vk::Device getDevice() const;
private:
    vk::UniqueInstance m_Instance;
    vk::UniqueSurfaceKHR m_Surface;
    vk::PhysicalDevice m_PhysicalDevice;
    vk::UniqueDevice m_Device;

    queue::QueueFamilyTable m_QueueTable;

    std::unique_ptr<VulkanSwapchain> m_Swapchain;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_VULKAN_CONTEXT_HPP_
