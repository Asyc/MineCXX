#include "engine/vulkan/render/buffer/vulkan_uniform_buffer.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "engine/vulkan/vulkan_context.hpp"

namespace engine::vulkan::render::buffer {

VulkanUniformBuffer::VulkanUniformBuffer(VulkanTransferManager* transferManager,
                                         VmaAllocator allocator,
                                         const VulkanRenderPipeline* pipeline,
                                         vk::DescriptorPool descriptorPool,
                                         size_t size,
                                         uint32_t set,
                                         uint32_t binding) {
    VkBufferCreateInfo bufferCreateInfo = vk::BufferCreateInfo({}, size, vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::SharingMode::eExclusive);

    VmaAllocationCreateInfo allocationCreateInfo{};
    allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    allocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    VmaAllocation allocation;
    VkBuffer buffer;
    VmaAllocationInfo allocationInfo;

    vmaCreateBuffer(allocator, &bufferCreateInfo, &allocationCreateInfo, &buffer, &allocation, &allocationInfo);

    m_Buffer = buffer;
    m_Allocation = {allocation, [allocator, buffer](VmaAllocation allocation){
        vmaDestroyBuffer(allocator, buffer, allocation);
    }};
    m_MappedPtr = allocationInfo.pMappedData;

    VmaAllocatorInfo allocatorInfo;
    vmaGetAllocatorInfo(allocator, &allocatorInfo);
    vk::Device device = allocatorInfo.device;
}

void VulkanUniformBuffer::write(size_t offset, const void* ptr, size_t length) {
    memcpy(reinterpret_cast<char*>(m_MappedPtr) + offset, ptr, length);
}

VulkanUniformDescriptor::VulkanUniformDescriptor(VulkanRenderPipeline* targetPipeline, uint32_t set) : m_Descriptor(targetPipeline->allocateDescriptorSetUnique(set)), m_Set(set) {

}

void VulkanUniformDescriptor::bind(uint32_t binding, const VulkanUniformBuffer* uniformBuffer) {
    vk::DescriptorBufferInfo descriptorBufferInfo(uniformBuffer->getBuffer(), 0, VK_WHOLE_SIZE);
    vk::WriteDescriptorSet writeDescriptorSet(*m_Descriptor, binding, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &descriptorBufferInfo, nullptr);
    m_Descriptor.getOwner().updateDescriptorSets(1, &writeDescriptorSet, 0, nullptr);
}

void VulkanUniformDescriptor::bindAll(uint32_t* bindings, const UniformBuffer* buffers, size_t count) {
    auto* writes = (vk::WriteDescriptorSet*) alloca(sizeof(vk::WriteDescriptorSet) * count);
    auto* bufferInfo = (vk::DescriptorBufferInfo*) alloca(sizeof(vk::DescriptorBufferInfo) * count);

    for (size_t i = 0; i < count; i++) {
        uint32_t binding = bindings[i];
        auto* vkBuffer = dynamic_cast<const VulkanUniformBuffer*>(&buffers[i]);

        writes[i] = vk::WriteDescriptorSet(*m_Descriptor, binding, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &bufferInfo[i], nullptr);
        bufferInfo[i] = vk::DescriptorBufferInfo(vkBuffer->getBuffer(), 0, VK_WHOLE_SIZE);
    }

    m_Descriptor.getOwner().updateDescriptorSets(count, writes, 0, nullptr);
}

}   // namespace engine::vulkan::render::buffer