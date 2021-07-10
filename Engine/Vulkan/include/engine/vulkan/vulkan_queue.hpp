#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_VULKAN_QUEUE_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_VULKAN_QUEUE_HPP_

#include <cstdint>
#include <optional>

#include <vulkan/vulkan.hpp>

namespace engine::render::vulkan::queue {

struct QueueFamilyEntry {
    uint32_t index = -1;
    vk::Queue handle = nullptr;

    uint32_t maxQueues;

    explicit operator vk::Queue() const {return handle;}
};

struct QueueFamilyTable {
    std::optional<QueueFamilyEntry> graphicsFamily;
    std::optional<QueueFamilyEntry> presentFamily;
};

}   // namespace engine::render::vulkan::queue

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_VULKAN_QUEUE_HPP_
