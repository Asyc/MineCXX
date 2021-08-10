#include "engine/vulkan/render/swapchain/vulkan_swapchain.hpp"

#include "engine/log.hpp"
#include "engine/vulkan/render/command/vulkan_command_buffer.hpp"

namespace engine::vulkan::render {

inline vk::PresentModeKHR mapSwapchainMode(Swapchain::SwapchainMode mode) {
  switch (mode) {
    case Swapchain::SwapchainMode::DOUBLE_BUFFER:
    case Swapchain::SwapchainMode::TRIPLE_BUFFER:return vk::PresentModeKHR::eImmediate;
    case Swapchain::SwapchainMode::DOUBLE_BUFFER_VSYNC:
    case Swapchain::SwapchainMode::TRIPLE_BUFFER_VSYNC:return vk::PresentModeKHR::eFifo;
    default: throw std::runtime_error("invalid swapchain mode");
  }
}

inline vk::Format findSupportedFormat(vk::PhysicalDevice device, const std::vector<vk::Format>& supported, vk::ImageTiling tiling, vk::FormatFeatureFlags features) {
  for (vk::Format format : supported) {
    vk::FormatProperties formatProperties = device.getFormatProperties(format);

    if (tiling == vk::ImageTiling::eLinear && (formatProperties.linearTilingFeatures & features) == features) {
      return format;
    } else if (tiling == vk::ImageTiling::eOptimal && (formatProperties.optimalTilingFeatures & features) == features) {
      return format;
    }
  }

  throw std::runtime_error("failed to find supported format");
}

void threadEntry(VulkanSwapchain*);

VulkanSwapchain::VulkanSwapchain(Swapchain::SwapchainMode modeHint,
                                 vk::SurfaceKHR surface,
                                 vk::PhysicalDevice physicalDevice,
                                 device::VulkanDevice* device,
                                 VmaAllocator allocator) : m_Owner(device), m_Allocator(allocator) {
  vk::SurfaceCapabilitiesKHR capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
  bool tripleBuffer = false;
  m_SupportedModes.insert(SwapchainMode::DOUBLE_BUFFER_VSYNC);
  if (capabilities.maxImageCount >= 3) {
    m_SupportedModes.insert(SwapchainMode::TRIPLE_BUFFER_VSYNC);
    tripleBuffer = true;
  }

  auto presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
  for (vk::PresentModeKHR presentMode : presentModes) {
    if (presentMode == vk::PresentModeKHR::eImmediate) {
      m_SupportedModes.insert(SwapchainMode::DOUBLE_BUFFER);
      if (tripleBuffer) m_SupportedModes.insert(SwapchainMode::TRIPLE_BUFFER);
    }
  }

  if (m_SupportedModes.find(modeHint) == m_SupportedModes.end()) {
    modeHint = m_SupportedModes.begin().operator*();
  }

  auto availableFormats = physicalDevice.getSurfaceFormatsKHR(surface);
  vk::SurfaceFormatKHR selectedFormat = availableFormats[0];
  for (const auto& format : availableFormats) {
    if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
      selectedFormat = format;
    }
  }

  m_Mode = modeHint;

  m_Owner = device;
  m_Surface = surface;
  m_GraphicsQueue = device->getQueueManager().getGraphicsQueue();

  m_PresentMode = mapSwapchainMode(m_Mode);
  m_SwapchainFormat = selectedFormat;
  m_SwapchainExtent = capabilities.currentExtent;

  uint32_t imageCount = tripleBuffer ? 3 : 2;
  m_SwapchainImageCountMin = imageCount < capabilities.minImageCount ? capabilities.minImageCount : imageCount;

  m_DepthBufferFormat = findSupportedFormat(physicalDevice,
                                            {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
                                            vk::ImageTiling::eOptimal,
                                            vk::FormatFeatureFlagBits::eDepthStencilAttachment);

  std::array<vk::AttachmentDescription, 2> attachments = {
      vk::AttachmentDescription(
          {},
          m_SwapchainFormat.format,
          vk::SampleCountFlagBits::e1,
          vk::AttachmentLoadOp::eLoad,
          vk::AttachmentStoreOp::eStore,
          vk::AttachmentLoadOp::eDontCare,
          vk::AttachmentStoreOp::eDontCare,
          vk::ImageLayout::eColorAttachmentOptimal,
          vk::ImageLayout::eColorAttachmentOptimal
      ),
      vk::AttachmentDescription(
          {},
          m_DepthBufferFormat,
          vk::SampleCountFlagBits::e1,
          vk::AttachmentLoadOp::eLoad,
          vk::AttachmentStoreOp::eStore,
          vk::AttachmentLoadOp::eDontCare,
          vk::AttachmentStoreOp::eDontCare,
          vk::ImageLayout::eDepthStencilAttachmentOptimal,
          vk::ImageLayout::eDepthStencilAttachmentOptimal
      )
  };

  vk::AttachmentReference attachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal);
  vk::AttachmentReference depthAttachmentReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

  vk::SubpassDescription subpassDescription(
      {},
      vk::PipelineBindPoint::eGraphics,
      {},
      {},
      1,
      &attachmentReference,
      nullptr,
      &depthAttachmentReference
  );

  vk::SubpassDependency subpassDependency(
      VK_SUBPASS_EXTERNAL,
      0,
      vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
      vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
      {},
      vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite
  );

  vk::RenderPassCreateInfo renderPassCreateInfo({}, attachments.size(), attachments.data(), 1, &subpassDescription, 1, &subpassDependency);
  m_RenderPass = device->getDevice().createRenderPassUnique(renderPassCreateInfo);

  vk::CommandPoolCreateInfo commandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, device->getQueueManager().getGraphicsQueueFamily().index);
  m_CommandPool = device->getDevice().createCommandPoolUnique(commandPoolCreateInfo);

  // After finally initializing all relevant class members, we can construct the swapchain
  createSwapchain();

  // Set our first render flight and initialize the image
  m_CurrentFlight = 0;
  setupImage();
}

VulkanSwapchain::~VulkanSwapchain() {
  m_Owner->getDevice().waitIdle();

  for (auto& flight : m_ResourceFlights) {
    flight.resources.clear();
  }
}

void VulkanSwapchain::addResourceFree(std::shared_ptr<void> resource) {
  m_ResourceFlights[m_CurrentFlight].resources.push_back(std::move(resource));
}

void VulkanSwapchain::nextImage() {
  auto& currentFlight = m_RenderFlights[m_CurrentFlight++];
  if (m_CurrentFlight == m_RenderFlights.size()) m_CurrentFlight = 0;

  if (m_SwapchainExtent.width == 0 || m_SwapchainExtent.height == 0) return;

  currentFlight.boundImage->present(m_GraphicsQueue, *m_Swapchain);

  setupImage();
}

void VulkanSwapchain::submitCommandBuffer(const DirectCommandBuffer& buffer) {
  if (m_SwapchainExtent.width == 0 || m_SwapchainExtent.height == 0) return;

  auto* vulkanPtr = dynamic_cast<const command::VulkanDirectCommandBuffer*>(&buffer);
  m_RenderFlights[m_CurrentFlight].submitCommandBuffer(m_Swapchain.getOwner(), vulkanPtr->getCommandBufferHandle(), vulkanPtr->getFence());
}

void VulkanSwapchain::submitCommandBuffer(const IndirectCommandBuffer& buffer) {
  if (m_SwapchainExtent.width == 0 || m_SwapchainExtent.height == 0) return;

  auto* vulkanPtr = dynamic_cast<const command::VulkanIndirectCommandBuffer*>(&buffer);
  m_RenderFlights[m_CurrentFlight].submitCommandBuffer(m_Swapchain.getOwner(), vulkanPtr->getCommandBufferHandle(), vulkanPtr->getFence());
}

void VulkanSwapchain::onResize(uint32_t width, uint32_t height) {
  m_Owner->getDevice().waitIdle();

  m_SwapchainExtent.width = width;
  m_SwapchainExtent.height = height;
  if (m_SwapchainExtent.width == 0 || m_SwapchainExtent.height == 0) return;

  createSwapchain();

  m_CurrentFlight = 0;
  setupImage();
}

Swapchain::SwapchainMode VulkanSwapchain::getSwapchainMode() const {
  return m_Mode;
}

const std::set<Swapchain::SwapchainMode>& VulkanSwapchain::getSupportedSwapchainModes() const {
  return m_SupportedModes;
}

uint32_t VulkanSwapchain::getCurrentFrameIndex() const {
  return m_CurrentFlight;
}

uint32_t VulkanSwapchain::getFrameCount() const {
  return m_RenderFlights.size();
}

std::pair<uint32_t, uint32_t> VulkanSwapchain::getSize() const {
  return {m_SwapchainExtent.width, m_SwapchainExtent.height};
}

vk::RenderPass VulkanSwapchain::getRenderPass() const {
  return *m_RenderPass;
}

void VulkanSwapchain::createSwapchain() {
  vk::SwapchainCreateInfoKHR createInfo(
      {},
      m_Surface,
      m_SwapchainImageCountMin,
      m_SwapchainFormat.format,
      m_SwapchainFormat.colorSpace,
      m_SwapchainExtent,
      1,
      vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst,
      vk::SharingMode::eExclusive,
      0,
      nullptr,
      vk::SurfaceTransformFlagBitsKHR::eIdentity,
      vk::CompositeAlphaFlagBitsKHR::eOpaque,
      m_PresentMode,
      VK_TRUE,
      *m_Swapchain
  );

  m_Swapchain = m_Owner->getDevice().createSwapchainKHRUnique(createInfo);

  auto images = m_Owner->getDevice().getSwapchainImagesKHR(*m_Swapchain);

  m_SwapchainImages.clear();

  for (uint32_t i = 0; i < images.size(); i++) {
    m_SwapchainImages.emplace_back(m_Owner, m_Allocator, images[i], i, m_SwapchainFormat.format, m_SwapchainExtent, m_DepthBufferFormat, *m_RenderPass, *m_CommandPool);
  }

  uint32_t flightCount;
  switch (m_Mode) {
    case SwapchainMode::DOUBLE_BUFFER:flightCount = 2;
      break;
    case SwapchainMode::TRIPLE_BUFFER:flightCount = 3;
      break;
    case SwapchainMode::DOUBLE_BUFFER_VSYNC:flightCount = 2;
      break;
    case SwapchainMode::TRIPLE_BUFFER_VSYNC:flightCount = 3;
      break;
  }

  m_RenderFlights.clear();
  for (uint32_t i = 0; i < flightCount; i++) {
    m_RenderFlights.emplace_back(m_Owner->getDevice());
  }

  m_ResourceFlights.clear();
  m_ResourceFlights.resize(m_RenderFlights.size());
}

void VulkanSwapchain::setupImage() {
  m_ResourceFlights[m_CurrentFlight].resources.clear();
  auto& currentFlight = m_RenderFlights[m_CurrentFlight];
  if (currentFlight.boundImage != nullptr) currentFlight.boundImage->wait();

  uint32_t index = m_Owner->getDevice().acquireNextImageKHR(*m_Swapchain, UINT64_MAX, *currentFlight.imageReadySemaphore).value;
  currentFlight.boundImage = &m_SwapchainImages[index];

  currentFlight.boundImage->setup(m_GraphicsQueue, *currentFlight.imageReadySemaphore);
}

vk::Framebuffer VulkanSwapchain::getCurrentFrame() const {
  return *m_RenderFlights[m_CurrentFlight].boundImage->framebuffer;
}

vk::Extent2D VulkanSwapchain::getExtent() const {
  return m_SwapchainExtent;
}
}   // namespace engine::vulkan::render