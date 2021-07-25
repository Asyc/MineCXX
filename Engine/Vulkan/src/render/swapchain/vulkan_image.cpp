#include "engine/vulkan/render/swapchain/vulkan_image.hpp"

namespace engine::vulkan::render {

Image::Image(device::VulkanDevice* device,
             vk::Image image,
             uint32_t imageIndex,
             vk::Format format,
             vk::Extent2D extent,
             vk::RenderPass renderPass,
             vk::CommandPool commandPool) : owner(device), imageIndex(imageIndex), activeSemaphoreCount() {
    vk::ImageViewCreateInfo imageViewCreateInfo(
        {},
        image,
        vk::ImageViewType::e2D,
        format,
        vk::ComponentMapping{},
        vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
    );

    imageView = owner->getDevice().createImageViewUnique(imageViewCreateInfo);

    vk::FramebufferCreateInfo framebufferCreateInfo({}, renderPass, 1, &*imageView, extent.width, extent.height, 1);
    framebuffer = owner->getDevice().createFramebufferUnique(framebufferCreateInfo);

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo(commandPool, vk::CommandBufferLevel::ePrimary, 2);
    auto buffers = owner->getDevice().allocateCommandBuffersUnique(commandBufferAllocateInfo);

    vk::ImageMemoryBarrier memoryBarrier(
        {},
        {},
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal,
        VK_QUEUE_FAMILY_IGNORED,
        VK_QUEUE_FAMILY_IGNORED,
        image,
        vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
    );

    clearScreenCommandBuffer = std::move(buffers[0]);
    clearScreenCommandBuffer->begin(vk::CommandBufferBeginInfo());
    clearScreenCommandBuffer->pipelineBarrier(
        vk::PipelineStageFlagBits::eAllCommands,
        vk::PipelineStageFlagBits::eAllCommands,
        {},
        {},
        {},
        {},
        {},
        1,
        &memoryBarrier
    );

    vk::ClearColorValue clearColorValue{};
    vk::ImageSubresourceRange subresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
    clearScreenCommandBuffer->clearColorImage(image, vk::ImageLayout::eTransferDstOptimal, &clearColorValue, 1, &subresourceRange);
    memoryBarrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
    memoryBarrier.newLayout = vk::ImageLayout::eColorAttachmentOptimal;
    clearScreenCommandBuffer->pipelineBarrier(
        vk::PipelineStageFlagBits::eAllCommands,
        vk::PipelineStageFlagBits::eAllCommands,
        {},
        {},
        {},
        {},
        {},
        1,
        &memoryBarrier
    );

    clearScreenCommandBuffer->end();

    memoryBarrier.oldLayout = vk::ImageLayout::eColorAttachmentOptimal;
    memoryBarrier.newLayout = vk::ImageLayout::ePresentSrcKHR;

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
        &memoryBarrier
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
    owner->getDevice().resetFences(fences.size(), fences.data());

    vk::SubmitInfo submitInfo(1, &imageAvailableSemaphore, &waitFlag, 1, &*clearScreenCommandBuffer, 0, &*clearScreenSemaphore);
    queue.submit(1, &submitInfo, *clearScreenFence);

    activeSemaphoreCount = 0;
}

void Image::present(vk::Queue queue, vk::SwapchainKHR swapchain) {
    auto* flags = reinterpret_cast<vk::PipelineStageFlags*>(alloca(sizeof(vk::PipelineStageFlags) * activeSemaphoreCount));
    for (uint32_t i = 0; i < activeSemaphoreCount; i++)
        flags[i] = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    vk::SubmitInfo submitInfo(activeSemaphoreCount, semaphores.data(), flags, 1, &*presentFormatCommandBuffer, 1, &*imageCompleteSemaphore);

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