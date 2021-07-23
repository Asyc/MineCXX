#include "engine/vulkan/render/buffer/vulkan_transfer_pool.hpp"

#include "engine/vulkan/device/vulkan_device.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace engine::vulkan::render::buffer {

VulkanTransferBuffer::VulkanTransferBuffer(VmaAllocator allocator, size_t size) : m_Size(size) {
    VkBufferCreateInfo bufferCreateInfo = vk::BufferCreateInfo({}, size, vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst, vk::SharingMode::eExclusive);

    VmaAllocationCreateInfo allocationCreateInfo{};
    allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

    VmaAllocation allocation;
    VkBuffer buffer;
    VmaAllocationInfo allocationInfo;

    vmaCreateBuffer(allocator, &bufferCreateInfo, &allocationCreateInfo, &buffer, &allocation, &allocationInfo);

    m_Buffer = buffer;
    m_Allocation = {allocation, [allocator, buffer](VmaAllocation allocation) {
        vmaDestroyBuffer(allocator, buffer, allocation);
    }};
    m_MappedPtr = allocationInfo.pMappedData;
}

void VulkanTransferBuffer::copy(const void* src, size_t length, size_t offset) const {
    memcpy(reinterpret_cast<int8_t*>(m_MappedPtr) + offset, src, length);
}

size_t VulkanTransferBuffer::getSize() const {
    return m_Size;
}

vk::Buffer VulkanTransferBuffer::getBuffer() const {
    return m_Buffer;
}

VulkanTransferPool::VulkanTransferPool(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device owner) {
    VmaAllocatorCreateInfo allocatorCreateInfo{
        NULL,
        physicalDevice,
        owner,
        false,
        nullptr,
        nullptr,
        NULL,
        nullptr,
        nullptr,
        nullptr,
        instance,
        VK_API_VERSION_1_2
    };

    VmaAllocator allocator;
    vmaCreateAllocator(&allocatorCreateInfo, &allocator);

    m_Allocator = {allocator, [](VmaAllocator allocator) {
        vmaDestroyAllocator(allocator);
    }};
}

VulkanTransferBuffer* VulkanTransferPool::acquire(size_t minimumSize) {
    std::unique_lock<std::mutex> lock(m_Mutex);

    auto it = m_Available.begin();
    while (it != m_Available.end()) {
        VulkanTransferBuffer* element = it.operator*();

        if (element->getSize() >= minimumSize) {
            m_Available.erase(it);
            return element;
        }

        it++;
    }

    auto& ref = m_Buffers.emplace_back(m_Allocator.get(), minimumSize);
    return &ref;
}

void VulkanTransferPool::release(VulkanTransferBuffer* element) {
    std::unique_lock<std::mutex> lock(m_Mutex);
    m_Available.push_back(element);
}

VulkanTransferBufferUnique VulkanTransferPool::acquireUnique(size_t minimumSize) {
    auto* acquired = acquire(minimumSize);
    return {this, acquired};
}

VulkanTransferBufferUnique::VulkanTransferBufferUnique(VulkanTransferPool* owner, VulkanTransferBuffer* buffer) : m_Owner(owner), m_Buffer(buffer) {

}

VulkanTransferBufferUnique::VulkanTransferBufferUnique(VulkanTransferBufferUnique&& rhs) noexcept: m_Owner(nullptr), m_Buffer(nullptr) {
    std::swap(m_Owner, rhs.m_Owner);
    std::swap(m_Buffer, rhs.m_Buffer);
}

VulkanTransferBufferUnique::~VulkanTransferBufferUnique() {
    if (m_Owner != nullptr && m_Buffer != nullptr) {
        m_Owner->release(m_Buffer);
    }
}

VulkanTransferBufferUnique& VulkanTransferBufferUnique::operator=(VulkanTransferBufferUnique&& rhs) noexcept {
    if (this != &rhs) {
        std::swap(m_Owner, rhs.m_Owner);
        std::swap(m_Buffer, rhs.m_Buffer);
    }

    return *this;
}

VulkanTransferManager::VulkanTransferManager(vk::Instance instance, vk::PhysicalDevice physicalDevice, device::VulkanDevice* device, uint32_t queueFamilyIndex) : m_TransferPool(instance,
                                                                                                                                                                                 physicalDevice,
                                                                                                                                                                                 device->getDevice()),
                                                                                                                                                                  m_Device(device),
                                                                                                                                                                  m_FlightIndex() {
    vk::CommandPoolCreateInfo commandPoolCreateInfo({vk::CommandPoolCreateFlagBits::eResetCommandBuffer | vk::CommandPoolCreateFlagBits::eTransient}, queueFamilyIndex);
    m_CommandPool = device->getDevice().createCommandPoolUnique(commandPoolCreateInfo);

    vk::CommandBufferAllocateInfo allocateInfo(*m_CommandPool, vk::CommandBufferLevel::ePrimary, BACKING_COUNT);
    auto buffers = device->getDevice().allocateCommandBuffersUnique(allocateInfo);

    m_Flights.resize(BACKING_COUNT);

    size_t index = 0;
    for (auto& it : m_Flights) {
        it.buffer = std::move(buffers[index++]);
        it.fence = device->getDevice().createFenceUnique(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
    }
}

VulkanTransferManager::~VulkanTransferManager() {
    m_CommandPool.getOwner().resetCommandPool(*m_CommandPool);
}

void VulkanTransferManager::submit(bool wait) {
    auto& currentFlight = m_Flights[m_FlightIndex++];
    if (m_FlightIndex == m_Flights.size()) m_FlightIndex = 0;
    if (currentFlight.empty) return;

    if (wait) m_CommandPool.getOwner().waitForFences(1, &*currentFlight.fence, VK_FALSE, UINT64_MAX);
    else if (m_CommandPool.getOwner().getFenceStatus(*currentFlight.fence) == vk::Result::eNotReady) return;
    m_CommandPool.getOwner().resetFences(1, &*currentFlight.fence);

    currentFlight.buffer->end();
    currentFlight.empty = true;

    vk::SubmitInfo submitInfo(0, nullptr, nullptr, 1, &*currentFlight.buffer);
    m_Device->getQueueManager().submitTransfer(1, &submitInfo, *currentFlight.fence);
}

void VulkanTransferManager::addTask(VulkanTransferBufferUnique src, vk::Buffer dst, size_t srcOffset, size_t dstOffset, size_t length) {
    auto& currentFlight = m_Flights[m_FlightIndex];
    m_CommandPool.getOwner().waitForFences(1, &*currentFlight.fence, VK_FALSE, UINT64_MAX);
    if (!currentFlight.pendingRelease.empty()) currentFlight.pendingRelease.clear();

    if (currentFlight.empty) {
        currentFlight.buffer->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
        currentFlight.empty = false;
    }

    vk::BufferCopy bufferCopy(srcOffset, dstOffset, length);
    currentFlight.buffer->copyBuffer(src->getBuffer(), dst, 1, &bufferCopy);
    currentFlight.pendingRelease.emplace_back(std::move(src));
}

void VulkanTransferManager::addTaskImage(VulkanTransferBufferUnique src, vk::Image dst, vk::BufferImageCopy imageCopy) {
    auto& currentFlight = m_Flights[m_FlightIndex];

    m_CommandPool.getOwner().waitForFences(1, &*currentFlight.fence, VK_FALSE, UINT64_MAX);
    if (currentFlight.empty) {
        currentFlight.buffer->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
        currentFlight.empty = false;
    }

    vk::ImageMemoryBarrier memoryBarrier(
        {},
        vk::AccessFlagBits::eTransferWrite,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal,
        VK_QUEUE_FAMILY_IGNORED,
        VK_QUEUE_FAMILY_IGNORED,
        dst,
        vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
    );

    currentFlight.buffer->pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, {}, 0, nullptr, 0, nullptr, 1, &memoryBarrier);
    currentFlight.buffer->copyBufferToImage(src->getBuffer(), dst, vk::ImageLayout::eTransferDstOptimal, 1, &imageCopy);

    memoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    memoryBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
    memoryBarrier.oldLayout = memoryBarrier.newLayout;
    memoryBarrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

    currentFlight.buffer->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, 0, nullptr, 0, nullptr, 1, &memoryBarrier);

    currentFlight.pendingRelease.emplace_back(std::move(src));
}

}   // namespace engine::vulkan::render::buffer