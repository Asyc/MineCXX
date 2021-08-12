#include "engine/vulkan/device/vulkan_device.hpp"

#include <vk_mem_alloc.h>

#include "engine/log.hpp"

namespace engine::vulkan::device {

// todo: change
inline thread_local static VulkanQueueFamily array[2];

inline vk::UniqueDevice createDevice(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) {
  struct QueueFamily {
    uint32_t index = -1;
    uint32_t maxQueueCount = -1;
  } graphics, present;

  uint32_t index = 0;
  for (const auto& queueFamilyProperties : physicalDevice.getQueueFamilyProperties()) {
    auto supportGraphics = queueFamilyProperties.queueFlags & vk::QueueFlagBits::eGraphics;
    bool supportPresent = physicalDevice.getSurfaceSupportKHR(index, surface);

    if (supportGraphics && supportPresent) {
      graphics = {index, queueFamilyProperties.queueCount};
      present = {index, queueFamilyProperties.queueCount};
      break;
    }

    if (supportGraphics) {
      graphics = {index, queueFamilyProperties.queueCount};
      continue;
    }

    if (supportPresent) {
      present = {index, queueFamilyProperties.queueCount};
    }

    index++;
  }

  float priorities[] = {0.0f, 1.0f};
  std::array<vk::DeviceQueueCreateInfo, 2> queuesCreateInfo{
      vk::DeviceQueueCreateInfo({}, graphics.index, graphics.maxQueueCount > 1 ? 2 : 1, priorities),
      vk::DeviceQueueCreateInfo({}, present.index, 1, priorities)
  };

  std::array<const char*, 1> deviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  vk::PhysicalDeviceFeatures features;
  features.geometryShader = true;
  vk::DeviceCreateInfo deviceCreateInfo(
      {},
      graphics.index != present.index ? 2 : 1,
      queuesCreateInfo.data(),
      0,
      nullptr,
      static_cast<uint32_t>(deviceExtensions.size()),
      deviceExtensions.data(),
      &features
  );

#if defined(MCE_DBG)
  std::array<const char*, 2> debugExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME};
  deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(debugExtensions.size());
  deviceCreateInfo.ppEnabledExtensionNames = debugExtensions.data();
#endif

  array[0] = VulkanQueueFamily{queuesCreateInfo[0].queueFamilyIndex, graphics.maxQueueCount};
  array[1] = graphics.index != present.index ? VulkanQueueFamily{queuesCreateInfo[1].queueFamilyIndex, graphics.maxQueueCount} : array[0];
  return physicalDevice.createDeviceUnique(deviceCreateInfo);
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

VulkanDevice::VulkanDevice(render::VulkanRenderContext* context, vk::Instance owner, vk::PhysicalDevice device, vk::SurfaceKHR surface)
    : m_Device(createDevice(device, surface)),
      m_QueueManager(context, array[0], array[1]),
      m_MemoryAllocator(createAllocator(owner, device, *m_Device), [](VmaAllocator allocator) { vmaDestroyAllocator(allocator); }),
      m_TransferManager(owner, device, this, m_QueueManager.getGraphicsQueueFamily().index) {}

}   // namespace engine::vulkan::device
