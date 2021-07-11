#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_SWAPCHAIN_VULKAN_SWAPCHAIN_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_SWAPCHAIN_VULKAN_SWAPCHAIN_HPP_

#include "engine/render/swapchain.hpp"

#include <atomic>
#include <vector>

#include "engine/vulkan/render/buffer/vulkan_image.hpp"
#include "engine/vulkan/render/buffer/vulkan_transfer_pool.hpp"
#include "engine/vulkan/vulkan_queue.hpp"
#include "vulkan_image.hpp"

namespace engine::render::vulkan {

class VulkanSwapchain : public Swapchain {
public:
    VulkanSwapchain(SwapchainMode modeHint, vk::SurfaceKHR surface, vk::PhysicalDevice, vk::Device device, const queue::QueueFamilyTable& queueFamilies);
    ~VulkanSwapchain() override;

    void nextImage() override;

    void submitCommandBuffer(const command::DirectCommandBuffer& buffer) override;
    void submitCommandBuffer(const command::IndirectCommandBuffer& buffer) override;

    void onResize(uint32_t width, uint32_t height) override;

    void addTransfer(render::buffer::vulkan::VulkanTransferBufferUnique src, vk::Buffer dst, vk::BufferCopy copy);
    void addTransferImage(render::buffer::vulkan::VulkanTransferBufferUnique src, vk::Image dst, vk::BufferImageCopy copy);
    void processTransferCommandBuffer();

    [[nodiscard]] SwapchainMode getSwapchainMode() const override;
    [[nodiscard]] const std::set<SwapchainMode>& getSupportedSwapchainModes() const override;

    [[nodiscard]] uint32_t getCurrentFrameIndex() const override;
    [[nodiscard]] uint32_t getFrameCount() const override;

    [[nodiscard]] std::tuple<uint32_t, uint32_t> getSize() const override;

    [[nodiscard]] vk::RenderPass getRenderPass() const;
    [[nodiscard]] vk::Framebuffer getCurrentFrame() const;

    [[nodiscard]] vk::Extent2D getExtent() const;

private:
    void createSwapchain();
    void setupImage();

    SwapchainMode m_Mode;
    std::set<SwapchainMode> m_SupportedModes;

    vk::Device m_Owner;
    vk::SurfaceKHR m_Surface;
    vk::Queue m_GraphicsQueue;

    vk::PresentModeKHR m_PresentMode;
    vk::SurfaceFormatKHR m_SwapchainFormat;
    vk::Extent2D m_SwapchainExtent;
    uint32_t m_SwapchainImageCountMin;

    vk::UniqueRenderPass m_RenderPass;
    vk::UniqueCommandPool m_CommandPool;

    vk::UniqueSwapchainKHR m_Swapchain;

    std::vector<Image> m_SwapchainImages;
    std::vector<ImageFlight> m_RenderFlights;
    size_t m_CurrentFlight;

    struct TransferFlight {
        std::atomic_bool empty{true};

        vk::UniqueFence fence;
        vk::UniqueCommandBuffer commandBuffer;

        std::vector<render::buffer::vulkan::VulkanTransferBufferUnique> releaseQueue;
    };

    vk::UniqueCommandPool m_TransferCommandPool;
    std::vector<TransferFlight> m_TransferFlights;

};

}   // namespace engine::render::vulkan


#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_SWAPCHAIN_VULKAN_SWAPCHAIN_HPP_
