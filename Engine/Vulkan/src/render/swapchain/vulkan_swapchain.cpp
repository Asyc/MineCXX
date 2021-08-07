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

void threadEntry(VulkanSwapchain*);

VulkanSwapchain::VulkanSwapchain(Swapchain::SwapchainMode modeHint,
                                 vk::SurfaceKHR surface,
                                 vk::PhysicalDevice physicalDevice,
                                 device::VulkanDevice* device,
                                 const device::VulkanQueueManager& queueManager) : m_Owner(device) {
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
  m_GraphicsQueue = queueManager.getGraphicsQueue();

  m_PresentMode = mapSwapchainMode(m_Mode);
  m_SwapchainFormat = selectedFormat;
  m_SwapchainExtent = capabilities.currentExtent;

  uint32_t imageCount = tripleBuffer ? 3 : 2;
  m_SwapchainImageCountMin = imageCount < capabilities.minImageCount ? capabilities.minImageCount : imageCount;

  vk::AttachmentDescription attachmentDescription(
      {},
      m_SwapchainFormat.format,
      vk::SampleCountFlagBits::e1,
      vk::AttachmentLoadOp::eLoad,
      vk::AttachmentStoreOp::eStore,
      vk::AttachmentLoadOp::eDontCare,
      vk::AttachmentStoreOp::eDontCare,
      vk::ImageLayout::eColorAttachmentOptimal,
      vk::ImageLayout::eColorAttachmentOptimal
  );

  vk::AttachmentReference attachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal);
  vk::SubpassDescription subpassDescription(
      {},
      vk::PipelineBindPoint::eGraphics,
      {},
      {},
      1,
      &attachmentReference
  );

  vk::SubpassDependency subpassDependency(
      VK_SUBPASS_EXTERNAL,
      0,
      vk::PipelineStageFlagBits::eColorAttachmentOutput,
      vk::PipelineStageFlagBits::eColorAttachmentOutput,
      {},
      vk::AccessFlagBits::eColorAttachmentWrite
  );

  vk::RenderPassCreateInfo renderPassCreateInfo({}, 1, &attachmentDescription, 1, &subpassDescription, 1, &subpassDependency);
  m_RenderPass = device->getDevice().createRenderPassUnique(renderPassCreateInfo);

  vk::CommandPoolCreateInfo commandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, queueManager.getGraphicsQueueFamily().index);
  m_CommandPool = device->getDevice().createCommandPoolUnique(commandPoolCreateInfo);

  // After finally initializing all relevant class members, we can construct the swapchain
  createSwapchain();

  // Set our first render flight and initialize the image
  m_CurrentFlight = 0;
  setupImage();
}

VulkanSwapchain::~VulkanSwapchain() {
  m_Owner->getDevice().waitIdle();
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
    m_SwapchainImages.emplace_back(m_Owner, images[i], i, m_SwapchainFormat.format, m_SwapchainExtent, *m_RenderPass, *m_CommandPool);
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
}

void VulkanSwapchain::setupImage() {
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