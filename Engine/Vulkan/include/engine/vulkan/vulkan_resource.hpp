#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_SRC_VULKAN_RESOURCE_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_SRC_VULKAN_RESOURCE_HPP_

#include <memory>
#include <mutex>
#include <vector>

#include <vulkan/vulkan.hpp>

namespace engine::vulkan {

class VulkanFenceProvider {
public:
    virtual vk::Fence getResourceFence() = 0;
};

class VulkanResource {
public:
    virtual ~VulkanResource();

    void pushConsumer(VulkanFenceProvider* consumer);
    void waitOnConsumers();
    void clearConsumers();
protected:
    VulkanResource() = default;

    vk::Device m_Owner;

    std::vector<VulkanFenceProvider*> m_Consumers;

    std::mutex m_ObjectLock;
};

}   // namespace engine::vulkan

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_SRC_VULKAN_RESOURCE_HPP_
