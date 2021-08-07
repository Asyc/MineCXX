#include "engine/vulkan/render/vulkan_resource.hpp"

#include <chrono>

namespace engine::vulkan::render {

VulkanResource::VulkanResource(vk::Device device) : m_Owner(device) {

}

VulkanResource::~VulkanResource() {
  if (m_Fences.empty()) return;

  auto* fences = (VkFence*) alloca(sizeof(VkFence) * m_Fences.size());

  for (size_t i = 0; i < m_Fences.size(); i++) {
    fences[i] = **m_Fences[i];
  }

  vkWaitForFences(m_Owner, m_Fences.size(), fences, VK_TRUE, UINT64_MAX);
}

void VulkanResource::pushFence(std::shared_ptr<vk::UniqueFence> fence) {
  m_Fences.emplace_back(std::move(fence));
}

void VulkanResource::waitFences() {
  auto* fences = (VkFence*) alloca(sizeof(VkFence) * m_Fences.size());

  for (size_t i = 0; i < m_Fences.size(); i++) {
    fences[i] = **m_Fences[i];
  }

  vkWaitForFences(m_Owner, m_Fences.size(), fences, VK_TRUE, UINT64_MAX);
}

}