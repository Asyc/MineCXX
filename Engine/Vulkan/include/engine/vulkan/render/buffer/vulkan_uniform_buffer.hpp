#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_UNIFORM_BUFFER_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_UNIFORM_BUFFER_HPP_

#include "engine/render/buffer/uniform_buffer.hpp"

#include <optional>

#include <vulkan/vulkan.hpp>

#include "engine/vulkan/render/buffer/vulkan_transfer_pool.hpp"
#include "engine/vulkan/render/vulkan_pipeline.hpp"

VK_DEFINE_HANDLE(VmaAllocation)

namespace engine::vulkan::render::buffer {

using namespace ::engine::render::buffer;

class IVulkanUniformBuffer {
public:
    [[nodiscard]] virtual vk::Buffer getBuffer() const = 0;
    [[nodiscard]] virtual vk::DescriptorSet getDescriptorSet() const = 0;
};

class VulkanUniformBuffer : public UniformBuffer, public IVulkanUniformBuffer {
public:
    VulkanUniformBuffer(VulkanTransferManager* transferManager,
                        VmaAllocator allocator,
                        const VulkanRenderPipeline* pipeline,
                        vk::DescriptorPool descriptorPool,
                        size_t size,
                        uint32_t set,
                        uint32_t binding);

    void write(size_t offset, const void* ptr, size_t length) override;

    vk::Buffer getBuffer() const override { return m_Buffer; }
    vk::DescriptorSet getDescriptorSet() const override {return *m_DescriptorSet;}
protected:
    vk::Buffer m_Buffer;
    std::unique_ptr<VmaAllocation_T, std::function<void(VmaAllocation)>> m_Allocation;
    vk::UniqueDescriptorSet m_DescriptorSet;

    void* m_MappedPtr;
};

class VulkanUniformDescriptor : public UniformDescriptor {
public:
    VulkanUniformDescriptor(VulkanRenderPipeline* targetPipeline, uint32_t set);

    void bind(uint32_t binding, const VulkanUniformBuffer* uniformBuffer);

    void bindAll(uint32_t* bindings, const UniformBuffer* buffers, size_t count) override;
    void bind(uint32_t binding, const UniformBuffer& buffer) override {
        bind(binding, dynamic_cast<const VulkanUniformBuffer*>(&buffer));
    }

    [[nodiscard]] vk::DescriptorSet getDescriptorSet() const {
        return *m_Descriptor;
    }

    [[nodiscard]] uint32_t getSetIndex() const {
        return m_Set;
    }
protected:
    vk::UniqueDescriptorSet m_Descriptor;
    uint32_t m_Set;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_BUFFER_VULKAN_UNIFORM_BUFFER_HPP_
