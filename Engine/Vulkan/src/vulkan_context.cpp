#include "engine/vulkan/vulkan_context.hpp"

#include <chrono>
#include <vector>

#define VMA_IMPLEMENTATION
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "engine/log.hpp"
#include "engine/vulkan/render/buffer/vulkan_image.hpp"
#include "engine/vulkan/render/buffer/vulkan_index_buffer.hpp"
#include "engine/vulkan/render/buffer/vulkan_vertex_buffer.hpp"
#include "engine/vulkan/render/command/vulkan_command_pool.hpp"
#include "engine/vulkan/render/swapchain/vulkan_swapchain.hpp"
#include "engine/vulkan/render/vulkan_pipeline.hpp"

namespace engine::vulkan::render {

vk::UniqueInstance createInstance() {
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

    return vk::createInstanceUnique(instanceCreateInfo);
}

vk::PhysicalDevice findPhysicalDevice(const std::vector<vk::PhysicalDevice>& devices) {
    vk::PhysicalDevice physicalDevice;
    for (const auto& device : devices) {
        vk::PhysicalDeviceProperties properties = device.getProperties();
        MCE_LOG_INFO("Render Device: {}", properties.deviceName);
        physicalDevice = device;
        break;
    }

    return physicalDevice;
}

inline VmaAllocator createAllocator(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device device) {
    VmaAllocatorCreateInfo allocatorCreateInfo{};
    allocatorCreateInfo.physicalDevice = physicalDevice;
    allocatorCreateInfo.device = device;
    allocatorCreateInfo.instance = instance;
    allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_2;

    VmaAllocator allocator;
    vmaCreateAllocator(&allocatorCreateInfo, &allocator);
    return allocator;
}

VulkanRenderContext::VulkanRenderContext(const Window& window, Swapchain::SwapchainMode modeHint)
    : m_Instance(createInstance()),
      m_Surface(window.createSurface(*m_Instance)),
      m_PhysicalDevice(findPhysicalDevice(m_Instance->enumeratePhysicalDevices())),
      m_Device(this, *m_Instance, m_PhysicalDevice, *m_Surface),
      m_MemoryAllocator(createAllocator(*m_Instance, m_PhysicalDevice, m_Device.getDevice()), [](VmaAllocator allocator) { vmaDestroyAllocator(allocator); }),
      m_Swapchain(modeHint, *m_Surface, m_PhysicalDevice, &m_Device, m_Device.getQueueManager()),
      m_TransferManager(*m_Instance, m_PhysicalDevice, &m_Device, m_Device.getQueueManager().getGraphicsQueueFamily().index) {

    std::array<vk::DescriptorPoolSize, 1> poolSizes = {
        vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 1)
    };
    m_DescriptorPool = m_Device.getDevice().createDescriptorPoolUnique(vk::DescriptorPoolCreateInfo({}, 4, poolSizes.size(), poolSizes.data()));
}

std::unique_ptr<buffer::Image> VulkanRenderContext::createImage(const File& path) {
    return std::make_unique<buffer::VulkanImage>(&m_TransferManager, m_MemoryAllocator.get(), path);
}

std::unique_ptr<command::CommandPool> VulkanRenderContext::createCommandPool() const {
    return std::make_unique<render::command::VulkanCommandPool>(m_Device.getDevice(), m_Device.getQueueManager().getGraphicsQueueFamily().index, false, &m_Swapchain);
}

std::unique_ptr<buffer::VertexBuffer> VulkanRenderContext::allocateVertexBuffer(size_t size) {
    return std::make_unique<buffer::VulkanVertexBuffer>(&m_TransferManager, m_MemoryAllocator.get(), size);
}

std::unique_ptr<buffer::IndexBuffer> VulkanRenderContext::allocateIndexBuffer(size_t size) {
    return std::make_unique<buffer::VulkanIndexBuffer>(&m_TransferManager, m_MemoryAllocator.get(), size);
}

std::unique_ptr<buffer::UniformBuffer> VulkanRenderContext::allocateUniformBuffer(const RenderPipeline& pipeline, size_t size) {
    auto* vkPipeline = dynamic_cast<const VulkanRenderPipeline*>(&pipeline);
    return std::make_unique<buffer::VulkanUniformBuffer>(&m_TransferManager, m_MemoryAllocator.get(), vkPipeline, *m_DescriptorPool, size, 0, 0);
}

std::unique_ptr<RenderPipeline> VulkanRenderContext::createRenderPipeline(const File& file) const {
    return std::make_unique<VulkanRenderPipeline>(m_Device.getDevice(), m_Swapchain.getRenderPass(), VulkanProgram(m_Device.getDevice(), file.getPath()));
}

void VulkanRenderContext::mouseButtonCallback(gui::input::MouseButton button, gui::input::MouseButtonAction action, double x, double y) {
    LOG_INFO("Button: {}, Action: {}, [{},{}]", button, action, x, y);
}

}   //namespace engine::render::vulkan
