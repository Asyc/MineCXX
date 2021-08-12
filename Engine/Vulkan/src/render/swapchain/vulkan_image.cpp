#include "engine/vulkan/render/swapchain/vulkan_image.hpp"

#include <vk_mem_alloc.h>

namespace engine::vulkan::render {

Image::Image(device::VulkanDevice* device,
             VmaAllocator allocator,
             vk::Image image,
             uint32_t imageIndex,
             vk::Format format,
             vk::Extent2D extent,
             vk::Format depthBufferFormat,
             vk::RenderPass renderPass,
             vk::CommandPool commandPool) : owner(device), imageIndex(imageIndex), activeSemaphoreCount() {

  VkImageCreateInfo depthImageCreateInfo = vk::ImageCreateInfo(
      {},
      vk::ImageType::e2D,
      depthBufferFormat,
      {extent.width, extent.height, 1},
      1,
      1,
      vk::SampleCountFlagBits::e1,
      vk::ImageTiling::eOptimal,
      vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferDst,
      vk::SharingMode::eExclusive,
      {},
      vk::ImageLayout::eUndefined
  );

  VmaAllocationCreateInfo depthAllocationCreateInfo{};
  depthAllocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  VkImage imageHandle;
  VmaAllocation allocationHandle;
  VmaAllocationInfo allocationInfo;
  vmaCreateImage(allocator, &depthImageCreateInfo, &depthAllocationCreateInfo, &imageHandle, &allocationHandle, &allocationInfo);

  depthImage = imageHandle;
  depthAllocation = {allocationHandle, [allocator, imageHandle](VmaAllocation allocation) {
    vmaDestroyImage(allocator, imageHandle, allocation);
  }};

  vk::ImageViewCreateInfo depthImageViewCreateInfo(
      {},
      imageHandle,
      vk::ImageViewType::e2D,
      depthBufferFormat,
      vk::ComponentMapping{},
      vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1)
  );

  depthImageView = device->getDevice().createImageViewUnique(depthImageViewCreateInfo);

  vk::ImageViewCreateInfo imageViewCreateInfo(
      {},
      image,
      vk::ImageViewType::e2D,
      format,
      vk::ComponentMapping{},
      vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
  );

  imageView = owner->getDevice().createImageViewUnique(imageViewCreateInfo);

  std::array<vk::ImageView, 2> attachments = {
      *imageView,
      *depthImageView
  };

  vk::FramebufferCreateInfo framebufferCreateInfo({}, renderPass, static_cast<uint32_t>(attachments.size()), attachments.data(), extent.width, extent.height, 1);
  framebuffer = owner->getDevice().createFramebufferUnique(framebufferCreateInfo);

  vk::CommandBufferAllocateInfo commandBufferAllocateInfo(commandPool, vk::CommandBufferLevel::ePrimary, 2);
  auto buffers = owner->getDevice().allocateCommandBuffersUnique(commandBufferAllocateInfo);

  std::array<vk::ImageMemoryBarrier, 2> memoryBarriers = {
      vk::ImageMemoryBarrier(
          {},
          vk::AccessFlagBits::eTransferWrite,
          vk::ImageLayout::eUndefined,
          vk::ImageLayout::eTransferDstOptimal,
          VK_QUEUE_FAMILY_IGNORED,
          VK_QUEUE_FAMILY_IGNORED,
          image,
          vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
      ),
      vk::ImageMemoryBarrier(
          {},
          vk::AccessFlagBits::eTransferWrite,
          vk::ImageLayout::eUndefined,
          vk::ImageLayout::eTransferDstOptimal,
          VK_QUEUE_FAMILY_IGNORED,
          VK_QUEUE_FAMILY_IGNORED,
          depthImage,
          vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1)
      ),
  };

  clearScreenCommandBuffer = std::move(buffers[0]);
  clearScreenCommandBuffer->begin(vk::CommandBufferBeginInfo());
  clearScreenCommandBuffer->pipelineBarrier(
      vk::PipelineStageFlagBits::eTopOfPipe,
      vk::PipelineStageFlagBits::eTransfer,
      {},
      {},
      {},
      {},
      {},
      static_cast<uint32_t>(memoryBarriers.size()),
      memoryBarriers.data()
  );

  vk::ClearColorValue clearColorValue{};
  vk::ImageSubresourceRange subresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
  clearScreenCommandBuffer->clearColorImage(image, vk::ImageLayout::eTransferDstOptimal, &clearColorValue, 1, &subresourceRange);

  vk::ClearDepthStencilValue clearDepthValue(1.0f, 0);
  vk::ImageSubresourceRange depthSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1);
  clearScreenCommandBuffer->clearDepthStencilImage(depthImage, vk::ImageLayout::eTransferDstOptimal, &clearDepthValue, 1, &depthSubresourceRange);

  /**
   *     barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
   */

  memoryBarriers[0].srcAccessMask = vk::AccessFlagBits::eTransferWrite;
  memoryBarriers[0].dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
  memoryBarriers[0].oldLayout = vk::ImageLayout::eTransferDstOptimal;
  memoryBarriers[0].newLayout = vk::ImageLayout::eColorAttachmentOptimal;

  memoryBarriers[1].srcAccessMask = vk::AccessFlagBits::eTransferWrite;
  memoryBarriers[1].dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
  memoryBarriers[1].oldLayout = vk::ImageLayout::eTransferDstOptimal;
  memoryBarriers[1].newLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

  clearScreenCommandBuffer->pipelineBarrier(
      vk::PipelineStageFlagBits::eTransfer,
      vk::PipelineStageFlagBits::eColorAttachmentOutput,
      {},
      {},
      {},
      {},
      {},
      1,
      &memoryBarriers[0]
  );

  clearScreenCommandBuffer->pipelineBarrier(
      vk::PipelineStageFlagBits::eTransfer,
      vk::PipelineStageFlagBits::eEarlyFragmentTests,
      {},
      {},
      {},
      {},
      {},
      1,
      &memoryBarriers[1]
      );

  clearScreenCommandBuffer->end();

  memoryBarriers[0].srcAccessMask = {};
  memoryBarriers[0].dstAccessMask = {};
  memoryBarriers[0].oldLayout = vk::ImageLayout::eColorAttachmentOptimal;
  memoryBarriers[0].newLayout = vk::ImageLayout::ePresentSrcKHR;

  presentFormatCommandBuffer = std::move(buffers[1]);
  presentFormatCommandBuffer->begin(vk::CommandBufferBeginInfo());
  presentFormatCommandBuffer->pipelineBarrier(
      vk::PipelineStageFlagBits::eBottomOfPipe,
      vk::PipelineStageFlagBits::eTopOfPipe,
      {},
      {},
      {},
      {},
      {},
      1,
      &memoryBarriers[0]
  );
  presentFormatCommandBuffer->end();

  // Creating synchronization objects
  clearScreenSemaphore = owner->getDevice().createSemaphoreUnique({});
  clearScreenFence = owner->getDevice().createFenceUnique({});

  imageCompleteSemaphore = owner->getDevice().createSemaphoreUnique({});
  imageCompleteFence = owner->getDevice().createFenceUnique(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
}

Image::~Image() {
  for (const auto& semaphore : semaphores)
    owner->getDevice().destroy(semaphore);
}

void Image::wait() {
  owner->getDevice().waitForFences(1, &*imageCompleteFence, VK_FALSE, UINT64_MAX);
}

void Image::setup(vk::Queue queue, vk::Semaphore imageAvailableSemaphore) {
  const vk::PipelineStageFlags waitFlag = vk::PipelineStageFlagBits::eColorAttachmentOutput;

  wait();

  std::array<vk::Fence, 2> fences = {*clearScreenFence, *imageCompleteFence};
  owner->getDevice().resetFences(static_cast<uint32_t>(fences.size()), fences.data());

  vk::SubmitInfo submitInfo(1, &imageAvailableSemaphore, &waitFlag, 1, &*clearScreenCommandBuffer, 0, &*clearScreenSemaphore);
  queue.submit(1, &submitInfo, *clearScreenFence);

  activeSemaphoreCount = 0;
}

void Image::present(vk::Queue queue, vk::SwapchainKHR swapchain) {
  auto* flags = reinterpret_cast<vk::PipelineStageFlags*>(alloca(sizeof(vk::PipelineStageFlags) * activeSemaphoreCount));
  for (uint32_t i = 0; i < activeSemaphoreCount; i++)
    flags[i] = vk::PipelineStageFlagBits::eColorAttachmentOutput;

  vk::SubmitInfo submitInfo(static_cast<uint32_t>(activeSemaphoreCount), semaphores.data(), flags, 1, &*presentFormatCommandBuffer, 1, &*imageCompleteSemaphore);

  queue.submit(1, &submitInfo, *imageCompleteFence);

  vk::PresentInfoKHR presentInfo(1, &*imageCompleteSemaphore, 1, &swapchain, &imageIndex, nullptr);
  queue.presentKHR(presentInfo);
}

vk::Semaphore Image::getSemaphore() {
  if (activeSemaphoreCount == semaphores.size()) {
    semaphores.push_back(owner->getDevice().createSemaphore(vk::SemaphoreCreateInfo()));
  }

  return semaphores[activeSemaphoreCount++];
}

ImageFlight::ImageFlight(vk::Device device) : boundImage(nullptr), imageReadySemaphore(device.createSemaphoreUnique({})) {

}

void ImageFlight::submitCommandBuffer(vk::Device device, vk::CommandBuffer buffer, vk::Fence fence) const {
  vk::Semaphore semaphore = boundImage->getSemaphore();
  vk::SubmitInfo submitInfo(0, nullptr, nullptr, 1, &buffer, 1, &semaphore);

  device.waitForFences(1, &*boundImage->clearScreenFence, VK_FALSE, UINT64_MAX);
  boundImage->owner->getQueueManager().submitGraphics(1, &submitInfo, fence);
}

}   // namespace engine::vulkan::render