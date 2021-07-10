#include "engine/vulkan/vulkan_resource.hpp"

namespace engine::vulkan {

VulkanResource::~VulkanResource() {

}

void VulkanResource::pushConsumer(VulkanFenceProvider* consumer) {
    std::unique_lock<std::mutex> lock(m_ObjectLock);
    m_Consumers.push_back(consumer);

}
void VulkanResource::waitOnConsumers() {
    std::unique_lock<std::mutex> lock(m_ObjectLock);

    auto* fences = (vk::Fence*) alloca(m_Consumers.size() * sizeof(vk::Fence));

    for (size_t i = 0; i < m_Consumers.size(); i++) {
        fences[i] = m_Consumers[i]->getResourceFence();
    }

    m_Owner.waitForFences(m_Consumers.size(), fences, VK_TRUE, UINT64_MAX);
}

void VulkanResource::clearConsumers() {
    std::unique_lock<std::mutex> lock(m_ObjectLock);
    m_Consumers.clear();
}

}