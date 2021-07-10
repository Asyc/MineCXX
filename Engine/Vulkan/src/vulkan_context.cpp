#include "engine/vulkan/vulkan_context.hpp"

#include <chrono>
#include <vector>

#include "engine/log.hpp"
#include "engine/vulkan/render/buffer/vulkan_index_buffer.hpp"
#include "engine/vulkan/render/buffer/vulkan_vertex_buffer.hpp"
#include "engine/vulkan/render/command/vulkan_command_pool.hpp"
#include "engine/vulkan/render/swapchain/vulkan_swapchain.hpp"
#include "engine/vulkan/render/vulkan_pipeline.hpp"

namespace engine::render::vulkan {

thread_local decltype(VulkanRenderContext::s_ThreadCommandPoolMap) VulkanRenderContext::s_ThreadCommandPoolMap;

VulkanRenderContext::VulkanRenderContext(const Window& window, Swapchain::SwapchainMode modeHint) {

    vk::ApplicationInfo applicationInfo(
        "Minecraft",
        VK_MAKE_VERSION(1, 0, 0),
        "MinecraftCXX-Engine",
        VK_MAKE_VERSION(1, 0, 0),
        VK_API_VERSION_1_2
    );

    auto[extensions, extensionCount] = Window::getExtensions();
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

    struct MemoryType {
        uint32_t index;
        size_t heapSize;
    };

    auto memoryProperties = m_PhysicalDevice.getMemoryProperties();

    std::optional<MemoryType> transferType, localType;
    uint32_t memoryTypeIndex = 0;

    // Loop through all memory types and assign "transferType" & "localType" based on heap sizes
    for (const auto& memoryType : memoryProperties.memoryTypes) {
        if (memoryTypeIndex + 1 == memoryProperties.memoryTypeCount) break;

        size_t heapSize = memoryProperties.memoryHeaps[memoryType.heapIndex].size;

        if (memoryType.propertyFlags & vk::MemoryPropertyFlagBits::eHostCoherent && memoryType.propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible) {
            if ((transferType.has_value() && heapSize > transferType->heapSize) || !transferType.has_value()) {
                transferType = {memoryTypeIndex, heapSize};
            }
        }

        if (memoryType.propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal) {
            if ((localType.has_value() && heapSize > localType->heapSize) || !localType.has_value()) {
                localType = {memoryTypeIndex, heapSize};
            }
        }

        memoryTypeIndex++;
    }

    m_TransferMemoryTypeIndex = transferType->index;
    m_LocalMemoryTypeIndex = localType->index;

    m_TransferPool = std::make_unique<buffer::vulkan::VulkanTransferPool>(*m_Device, m_TransferMemoryTypeIndex);

}

std::unique_ptr<command::CommandPool> VulkanRenderContext::createCommandPool() const {
    return std::make_unique<render::command::vulkan::VulkanCommandPool>(*m_Device, m_QueueTable.graphicsFamily->index, false, m_Swapchain.get());
}

command::CommandPool& VulkanRenderContext::getThreadCommandPool() const {
    const auto& it = s_ThreadCommandPoolMap.find(this);
    if (it != s_ThreadCommandPoolMap.end()) return *it->second;

    const auto& it2 = s_ThreadCommandPoolMap.emplace(this, std::make_unique<render::command::vulkan::VulkanCommandPool>(*m_Device, m_QueueTable.graphicsFamily->index, false, m_Swapchain.get()));
    return *it2.first->second;
}

std::unique_ptr<buffer::VertexBuffer> VulkanRenderContext::allocateVertexBuffer(size_t size) {
    return std::make_unique<buffer::vulkan::VulkanVertexBuffer>(*m_Device, m_LocalMemoryTypeIndex, size, this, m_TransferPool.get());
}

std::unique_ptr<buffer::IndexBuffer> VulkanRenderContext::allocateIndexBuffer(size_t size) {
    return std::make_unique<buffer::vulkan::VulkanIndexBuffer>(*m_Device, m_LocalMemoryTypeIndex, size, this, m_TransferPool.get());
}

std::unique_ptr<RenderPipeline> VulkanRenderContext::createRenderPipeline(const File& file) const {
    return std::make_unique<VulkanRenderPipeline>(*m_Device, m_Swapchain->getRenderPass(), std::make_shared<VulkanProgram>(*m_Device, file.getPath()));
}

Swapchain& VulkanRenderContext::getSwapchain() {
    return *m_Swapchain;
}
const Swapchain& VulkanRenderContext::getSwapchain() const {
    return *m_Swapchain;
}

VulkanSwapchain& VulkanRenderContext::getSwapchainVulkan() {
    return *m_Swapchain;
}

}   //namespace engine::render::vulkan
