#include "engine/vulkan/vulkan_context.hpp"

#include <vector>

#include "engine/log.hpp"
#include "engine/vulkan/render/command/vulkan_command_pool.hpp"
#include "engine/vulkan/render/swapchain/vulkan_swapchain.hpp"
#include "engine/vulkan/render/vulkan_pipeline.hpp"

namespace engine::render::vulkan {

VulkanRenderContext::VulkanRenderContext(const Window& window, Swapchain::SwapchainMode modeHint) {

    vk::ApplicationInfo applicationInfo(
        "Minecraft",
        VK_MAKE_VERSION(1, 0, 0),
        "MinecraftCXX-Engine",
        VK_MAKE_VERSION(1, 0, 0),
        VK_API_VERSION_1_2
    );

    auto [extensions, extensionCount] = Window::getExtensions();
    vk::InstanceCreateInfo instanceCreateInfo({}, &applicationInfo, 0, nullptr, extensionCount, extensions);

#ifdef MCE_DBG
    const char* validationLayer = "VK_LAYER_KHRONOS_validation";
    instanceCreateInfo.enabledLayerCount = 1;
    instanceCreateInfo.ppEnabledLayerNames = &validationLayer;
    MCE_LOG_DEBUG("Validation Layer: {}", validationLayer);
#endif

    m_Instance = vk::createInstanceUnique(instanceCreateInfo);
    m_Surface = window.createSurface(*m_Instance);
    // Now we chose a target device to do rendering on
    std::vector<vk::PhysicalDevice> devices = m_Instance->enumeratePhysicalDevices();
    for (const auto& device : devices) {
        vk::PhysicalDeviceProperties properties = device.getProperties();
        MCE_LOG_INFO("Render Device: {}", properties.deviceName);
        m_PhysicalDevice = device;
        break;
    }

    std::vector<vk::QueueFamilyProperties> queueFamilies = m_PhysicalDevice.getQueueFamilyProperties();
    uint32_t index = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (!m_QueueTable.graphicsFamily.has_value() && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
            m_QueueTable.graphicsFamily = queue::QueueFamilyEntry{index, {}, queueFamily.queueCount};
        }

        if (!m_QueueTable.presentFamily.has_value() && m_PhysicalDevice.getSurfaceSupportKHR(index, *m_Surface)) {
            m_QueueTable.presentFamily = queue::QueueFamilyEntry{index, {}, queueFamily.queueCount};

        }

        index++;
    }

    MCE_LOG_DEBUG("Graphics Queue: {}, Queue Count: {}", m_QueueTable.graphicsFamily->index, m_QueueTable.graphicsFamily->maxQueues);

    float priority = 1.0f;
    std::array<vk::DeviceQueueCreateInfo, 2> queues = {
        vk::DeviceQueueCreateInfo({}, m_QueueTable.graphicsFamily->index, 1, &priority),
        vk::DeviceQueueCreateInfo({}, m_QueueTable.presentFamily->index, 1, &priority)
    };

    const char* swapchainExtension = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    vk::PhysicalDeviceFeatures features;
    vk::DeviceCreateInfo deviceCreateInfo({}, 1, queues.data(), 0, nullptr, 1, &swapchainExtension, &features);

    // Check if the graphics and present queue families are different, if so, create a queue in both
    if (m_QueueTable.graphicsFamily->index != m_QueueTable.presentFamily->index) {
        deviceCreateInfo.queueCreateInfoCount = 2;
    }

    m_Device = m_PhysicalDevice.createDeviceUnique(deviceCreateInfo);
    m_QueueTable.graphicsFamily->handle = m_Device->getQueue(m_QueueTable.graphicsFamily->index, 0);
    m_QueueTable.presentFamily->handle = m_Device->getQueue(m_QueueTable.graphicsFamily->index, 0);

    m_Swapchain = std::make_unique<VulkanSwapchain>(modeHint, *m_Surface, m_PhysicalDevice, *m_Device, m_QueueTable);
}

std::unique_ptr<command::CommandPool> VulkanRenderContext::createCommandPool() const {
    return std::make_unique<render::command::vulkan::VulkanCommandPool>(*m_Device, m_QueueTable.graphicsFamily->index, m_Swapchain.get());
}

std::unique_ptr<RenderPipeline> VulkanRenderContext::createRenderPipeline(const File& file) const {
    return std::make_unique<VulkanRenderPipeline>(*m_Device, std::make_shared<VulkanProgram>(*m_Device, file.getPath()));
}

Swapchain& VulkanRenderContext::getSwapchain() {
    return *m_Swapchain;
}
const Swapchain& VulkanRenderContext::getSwapchain() const {
    return *m_Swapchain;
}

}   //namespace engine::render::vulkan
