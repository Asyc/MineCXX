#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_COMMAND_VULKAN_COMMAND_POOL_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_COMMAND_VULKAN_COMMAND_POOL_HPP_

#include "engine/render/command/command_pool.hpp"

#include <vulkan/vulkan.hpp>

#include "engine/vulkan/render/swapchain/vulkan_swapchain.hpp"

namespace engine::vulkan::render::command {

using namespace ::engine::render::command;

class VulkanCommandPool : public CommandPool {
 public:
  VulkanCommandPool(vk::Device device, uint32_t queueFamilyIndex, bool transient, const VulkanSwapchain* swapchain);

  std::unique_ptr<DirectCommandBuffer> allocateDirectCommandBuffer() override;
  std::unique_ptr<IndirectCommandBuffer> allocateIndirectCommandBuffer() override;

  std::unique_ptr<CommandList> allocateCommandList() override;
  std::unique_ptr<ImmutableCommandList> allocateCommandListImmutable() override;

  [[nodiscard]] vk::CommandPool getCommandPool() const;
 private:
  vk::UniqueCommandPool m_CommandPool;

  const VulkanSwapchain* m_SwapchainHandle;
};

}   //namespace engine::vulkan::render::command

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_COMMAND_VULKAN_COMMAND_POOL_HPP_
