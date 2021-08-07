#include "engine/vulkan/render/vulkan_descriptor.hpp"

#include "engine/vulkan/render/vulkan_pipeline.hpp"

namespace engine::vulkan::render {

VulkanUniformDescriptor::VulkanUniformDescriptor(VulkanRenderPipeline* targetPipeline, uint32_t set) : m_Descriptor(targetPipeline->allocateDescriptorSetUnique(set)), m_Set(set) {

}

void VulkanUniformDescriptor::bind(uint32_t binding, const IDescriptorResource& buffer) {
  bind(binding, dynamic_cast<const IVulkanDescriptorResource*>(&buffer));
}

void VulkanUniformDescriptor::bind(uint32_t binding, const IVulkanDescriptorResource* resource) {
  vk::DescriptorBufferInfo bufferInfo(resource->getBuffer(), 0, VK_WHOLE_SIZE);
  vk::DescriptorImageInfo imageInfo(resource->getSampler(), resource->getImageView(), vk::ImageLayout::eShaderReadOnlyOptimal);

  vk::WriteDescriptorSet writeDescriptorSet(*m_Descriptor, binding, 0, 1, resource->getDescriptorType(), &imageInfo, &bufferInfo, nullptr);
  m_Descriptor.getOwner().updateDescriptorSets(1, &writeDescriptorSet, 0, nullptr);
}

void VulkanUniformDescriptor::bindAll(uint32_t* bindings, const IDescriptorResource* resources, size_t count) {
  /*auto* writes = (vk::WriteDescriptorSet*) alloca(sizeof(vk::WriteDescriptorSet) * count);
  auto* bufferInfo = (vk::DescriptorBufferInfo*) alloca(sizeof(vk::DescriptorBufferInfo) * count);

  for (size_t i = 0; i < count; i++) {
      uint32_t binding = bindings[i];
      auto* vkBuffer = dynamic_cast<const VulkanUniformBuffer*>(&buffers[i]);

      writes[i] = vk::WriteDescriptorSet(*m_Descriptor, binding, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &bufferInfo[i], nullptr);
      bufferInfo[i] = vk::DescriptorBufferInfo(vkBuffer->getBuffer(), 0, VK_WHOLE_SIZE);
  }

  m_Descriptor.getOwner().updateDescriptorSets(count, writes, 0, nullptr);*/
}

}   // namespace engine::vulkan::render