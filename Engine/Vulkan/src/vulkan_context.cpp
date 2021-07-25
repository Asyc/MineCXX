#include "engine/vulkan/vulkan_context.hpp"

#include <chrono>
#include <vector>

#define VMA_IMPLEMENTATION
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "engine/log.hpp"
#include "engine/vulkan/render/buffer/vulkan_index_buffer.hpp"

#ifdef MCE_DBG

static VkDebugReportCallbackEXT g_DebugReportCallback;
static VkDebugUtilsMessengerEXT g_DebugMessenger;

VKAPI_ATTR VkBool32 VKAPI_CALL vulkanDebugCallback(VkDebugReportFlagsEXT flags,
                                                   VkDebugReportObjectTypeEXT objectType,
                                                   uint64_t object,
                                                   size_t location,
                                                   int32_t messageCode,
                                                   const char* pLayerPrefix,
                                                   const char* pMessage,
                                                   void* pUserData) {

    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
        MCE_LOG_DEBUG("[SHADER DEBUG] {}", pMessage);
    }

    return false;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    switch (messageSeverity) {

    }


    LOG_ERROR("Validation Layer: {}", pCallbackData->pMessage);

    return VK_FALSE;
}

#endif

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
    const char** debugExtensions = (const char**) (alloca(sizeof(const char*) * (extensionCount + 2)));
    memcpy(debugExtensions, extensions, (extensionCount + 1) * sizeof(const char*));
    debugExtensions[extensionCount] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    debugExtensions[extensionCount + 1] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;

    instanceCreateInfo.enabledExtensionCount += 2;
    instanceCreateInfo.ppEnabledExtensionNames = debugExtensions;

    const char* validationLayer = "VK_LAYER_KHRONOS_validation";
    instanceCreateInfo.enabledLayerCount = 1;
    instanceCreateInfo.ppEnabledLayerNames = &validationLayer;
    MCE_LOG_DEBUG("Validation Layer: {}", validationLayer);
#endif
    std::array<vk::ValidationFeatureEnableEXT, 1> enabledValidationFeatures = {
        vk::ValidationFeatureEnableEXT::eDebugPrintf
    };

    vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT, vk::ValidationFeaturesEXT> structureChain{
        instanceCreateInfo,
        vk::DebugUtilsMessengerCreateInfoEXT({}, vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning, vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral, debugCallback, nullptr),
        vk::ValidationFeaturesEXT(enabledValidationFeatures.size(), enabledValidationFeatures.data()),
    };

    vk::UniqueInstance instance = vk::createInstanceUnique(structureChain.get<vk::InstanceCreateInfo>());
#ifdef MCE_DBG
    VkInstance vulkanInstance = *instance;

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vulkanInstance, "vkCreateDebugUtilsMessengerEXT");

    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
    vk::DebugUtilsMessageTypeFlagsEXT typeFlags = vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;

    VkDebugUtilsMessengerCreateInfoEXT
    createInfoExt = vk::DebugUtilsMessengerCreateInfoEXT({}, severityFlags, vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral, debugCallback, nullptr);
    func(vulkanInstance, &createInfoExt, nullptr, &g_DebugMessenger);

    VkDebugReportCallbackCreateInfoEXT debugReportCallbackCreateInfo = vk::DebugReportCallbackCreateInfoEXT({}, vulkanDebugCallback, nullptr);

    PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback = VK_NULL_HANDLE;
    auto* ptr = vkGetInstanceProcAddr(vulkanInstance, "vkCreateDebugReportCallbackEXT");
    CreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT) ptr;

    VkDebugReportCallbackEXT handle;
    CreateDebugReportCallback(vulkanInstance, &debugReportCallbackCreateInfo, nullptr, &handle);

    g_DebugReportCallback = handle;
#endif

    return std::move(instance);
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

VulkanRenderContext::VulkanRenderContext(const Window& window, const Directory& resourceDirectory, Swapchain::SwapchainMode modeHint)
    : m_Instance(createInstance()),
      m_Surface(window.createSurface(*m_Instance)),
      m_PhysicalDevice(findPhysicalDevice(m_Instance->enumeratePhysicalDevices())),
      m_Device(this, *m_Instance, m_PhysicalDevice, *m_Surface),
      m_MemoryAllocator(createAllocator(*m_Instance, m_PhysicalDevice, m_Device.getDevice()), [](VmaAllocator allocator) { vmaDestroyAllocator(allocator); }),
      m_Swapchain(modeHint, *m_Surface, m_PhysicalDevice, &m_Device, m_Device.getQueueManager()),
      m_TransferManager(*m_Instance, m_PhysicalDevice, &m_Device, m_Device.getQueueManager().getGraphicsQueueFamily().index),
      m_FontRenderer(*this, File(resourceDirectory.getPath() + "font/glyph_sizes.bin"), Directory(resourceDirectory.getPath() + "/textures/font")) {

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
