#include "engine/vulkan/vulkan_context.hpp"

#include <chrono>
#include <vector>

#include <spdlog/sinks/basic_file_sink.h>

#define VMA_IMPLEMENTATION
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "engine/log.hpp"
#include "engine/vulkan/render/buffer/vulkan_index_buffer.hpp"

#ifdef MCE_DBG

thread_local static VkDebugUtilsMessengerEXT g_DebugMessenger;
thread_local static VkDebugReportCallbackEXT g_DebugReportCallback;

static spdlog::sink_ptr g_VulkanLogFile;
static std::shared_ptr<spdlog::logger> g_VulkanLogger;

VKAPI_ATTR VkBool32 VKAPI_CALL vulkanDebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT, uint64_t, size_t, int32_t, const char*, const char* pMessage, void* pUserData) {
    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT || flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
        g_VulkanLogger->error("{}", pMessage);
        return false;
    }

    g_VulkanLogger->debug("{}", pMessage);

    return false;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT,
                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                             void* pUserData) {
    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:g_VulkanLogger->error("{}", pCallbackData->pMessage);
            break;
        default:g_VulkanLogger->debug("{}", pCallbackData->pMessage);
            break;
    }

    return VK_FALSE;
}

#endif

namespace engine::vulkan::render {

thread_local std::unordered_map<VulkanRenderContext*, command::VulkanCommandPool> VulkanRenderContext::s_ThreadCommandPoolMap;

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

    std::array<vk::ValidationFeatureEnableEXT, 1> enabledValidationFeatures = {
        vk::ValidationFeatureEnableEXT::eDebugPrintf
    };

    vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT, vk::ValidationFeaturesEXT> structureChain{
        instanceCreateInfo,
        vk::DebugUtilsMessengerCreateInfoEXT({}, vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning, vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral, debugCallback, nullptr),
        vk::ValidationFeaturesEXT(enabledValidationFeatures.size(), enabledValidationFeatures.data()),
    };

    g_VulkanLogFile = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/vulkan_latest.log");
    g_VulkanLogger = engine::logging::createLoggerDetached("Vulkan");
    g_VulkanLogger->sinks().push_back(g_VulkanLogFile);

    vk::UniqueInstance instance = vk::createInstanceUnique(structureChain.get<vk::InstanceCreateInfo>());
    VkInstance vulkanInstance = *instance;

    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
    vk::DebugUtilsMessageTypeFlagsEXT typeFlags = vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
    VkDebugUtilsMessengerCreateInfoEXT createInfoExt = vk::DebugUtilsMessengerCreateInfoEXT({}, severityFlags, vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral, debugCallback, nullptr);

    auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vulkanInstance, "vkCreateDebugUtilsMessengerEXT");
    vkCreateDebugUtilsMessengerEXT(vulkanInstance, &createInfoExt, nullptr, &g_DebugMessenger);

    vk::DebugReportFlagsEXT
        debugReportFlags = vk::DebugReportFlagBitsEXT::eWarning | vk::DebugReportFlagBitsEXT::ePerformanceWarning | vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eDebug;
    VkDebugReportCallbackCreateInfoEXT debugReportCallbackCreateInfo = vk::DebugReportCallbackCreateInfoEXT(debugReportFlags, vulkanDebugCallback, nullptr);

    auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(vulkanInstance, "vkCreateDebugReportCallbackEXT");
    vkCreateDebugReportCallbackEXT(vulkanInstance, &debugReportCallbackCreateInfo, nullptr, &g_DebugReportCallback);
    return std::move(instance);
#else
    return vk::createInstanceUnique(instanceCreateInfo);
#endif
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

VulkanRenderContext::VulkanRenderContext(Window& window, const Directory& resourceDirectory, Swapchain::SwapchainMode modeHint)
    : m_Window(&window),
      m_Instance(createInstance()),
      m_Surface(window.createSurface(*m_Instance)),
      m_PhysicalDevice(findPhysicalDevice(m_Instance->enumeratePhysicalDevices())),
      m_Device(this, *m_Instance, m_PhysicalDevice, *m_Surface),
      m_MemoryAllocator(createAllocator(*m_Instance, m_PhysicalDevice, m_Device.getDevice()), [](VmaAllocator allocator) { vmaDestroyAllocator(allocator); }),
      m_Swapchain(modeHint, *m_Surface, m_PhysicalDevice, &m_Device, m_Device.getQueueManager()),
      m_TransferManager(*m_Instance, m_PhysicalDevice, &m_Device, m_Device.getQueueManager().getGraphicsQueueFamily().index),
      m_Pipelines(),
      m_FontRenderer(*this, File(resourceDirectory.getPath() + "font/glyph_sizes.bin"), Directory(resourceDirectory.getPath() + "/textures/font")) {

#ifdef MCE_DBG
    VkInstance instance = *m_Instance;
    m_DebugMessenger = {g_DebugMessenger, [instance](VkDebugUtilsMessengerEXT messenger) {
        auto vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        vkDestroyDebugUtilsMessengerEXT(instance, messenger, nullptr);
    }};

    m_DebugReportCallback = {g_DebugReportCallback, [instance](VkDebugReportCallbackEXT debugReportCallback) {
        auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
        vkDestroyDebugReportCallbackEXT(instance, debugReportCallback, nullptr);
    }};
#endif
    std::array<vk::DescriptorPoolSize, 1> poolSizes = {
        vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 1)
    };

    m_DescriptorPool = m_Device.getDevice().createDescriptorPoolUnique(vk::DescriptorPoolCreateInfo({}, 4, poolSizes.size(), poolSizes.data()));
}

std::shared_ptr<buffer::Image> VulkanRenderContext::createImage(const File& path) {
    return std::make_shared<buffer::VulkanImage>(&m_TransferManager, m_MemoryAllocator.get(), path);
}

std::unique_ptr<command::CommandPool> VulkanRenderContext::createCommandPool() const {
    return std::make_unique<render::command::VulkanCommandPool>(m_Device.getDevice(), m_Device.getQueueManager().getGraphicsQueueFamily().index, false, &m_Swapchain);
}

command::CommandPool& VulkanRenderContext::getThreadCommandPool() {
    auto it = s_ThreadCommandPoolMap.find(this);
    if (it == s_ThreadCommandPoolMap.end()) {
        auto ref = s_ThreadCommandPoolMap.emplace(this, command::VulkanCommandPool(m_Device.getDevice(), m_Device.getQueueManager().getGraphicsQueueFamily().index, false, &m_Swapchain));
        return ref.first->second;
    }

    return it->second;
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

std::shared_ptr<RenderPipeline> VulkanRenderContext::createRenderPipeline(const File& file) {
    auto it = m_Pipelines.find(file.getFullPath());
    if (it == m_Pipelines.end() || it->second.expired()) {
        auto ptr = std::make_shared<VulkanRenderPipeline>(m_Device.getDevice(), m_Swapchain.getRenderPass(), VulkanProgram(m_Device.getDevice(), file.getPath()));
        if (it == m_Pipelines.end()) {
            m_Pipelines.emplace(file.getFullPath(), std::weak_ptr<VulkanRenderPipeline>(ptr));
        } else {
            it->second = ptr;
        }
        return std::move(ptr);
    }

    return std::shared_ptr<RenderPipeline>{it->second};
}

void VulkanRenderContext::mouseButtonCallback(gui::input::MouseButton button, gui::input::MouseButtonAction action, double x, double y) {
    LOG_INFO("Button: {}, Action: {}, [{},{}]", button, action, x, y);
}

}   //namespace engine::render::vulkan
