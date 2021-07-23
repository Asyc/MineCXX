#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_VULKAN_DESCRIPTOR_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_VULKAN_DESCRIPTOR_HPP_

#include "engine/render/descriptor.hpp"

#include <vulkan/vulkan.hpp>

namespace engine::vulkan::render {

using namespace ::engine::render;

class VulkanRenderPipeline;

class IVulkanDescriptorResource {
public:
    [[nodiscard]] virtual vk::DescriptorType getDescriptorType() const = 0;

    [[nodiscard]] virtual vk::Buffer getBuffer() const {return {};}

    [[nodiscard]] virtual vk::Sampler getSampler() const {return {};}
    [[nodiscard]] virtual vk::ImageView getImageView() const {return {};}
};

class VulkanUniformDescriptor : public UniformDescriptor {
public:
    VulkanUniformDescriptor(VulkanRenderPipeline* targetPipeline, uint32_t set);

    void bind(uint32_t binding, const IVulkanDescriptorResource* resource);

    void bind(uint32_t binding, const IDescriptorResource& buffer) override;
    void bindAll(uint32_t* bindings, const IDescriptorResource* resources, size_t count) override;

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

}   // namespace engine::vulkan::render

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_VULKAN_DESCRIPTOR_HPP_
