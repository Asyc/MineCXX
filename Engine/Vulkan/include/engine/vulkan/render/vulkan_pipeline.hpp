#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_VULKAN_PIPELINE_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_VULKAN_PIPELINE_HPP_

#include "engine/render/pipeline.hpp"

#include <memory>
#include <string_view>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "engine/file.hpp"

namespace engine::vulkan::render {

using namespace ::engine::render;

class VulkanProgram;

class VulkanProgram : public Program {
public:
    VulkanProgram(vk::Device device, const std::string_view& program);

    [[nodiscard]] const std::vector<vk::PipelineShaderStageCreateInfo>& getStages() const;
    [[nodiscard]] vk::PipelineLayout getPipelineLayout() const;

    [[nodiscard]] const std::vector<vk::VertexInputBindingDescription>& getInputBindings() const;
    [[nodiscard]] const std::vector<vk::VertexInputAttributeDescription>& getInputAttributes() const;

    [[nodiscard]] const std::vector<vk::UniqueDescriptorSetLayout>& getDescriptorSetLayoutTable() const { return m_DescriptorSetLayoutTable; }
private:
    friend class VulkanRenderPipeline;

    vk::UniqueShaderModule m_Vertex, m_Fragment;
    std::vector<vk::PipelineShaderStageCreateInfo> m_Stages;
    vk::UniquePipelineLayout m_PipelineLayout;

    std::vector<vk::VertexInputBindingDescription> m_InputBindings;
    std::vector<vk::VertexInputAttributeDescription> m_InputAttributes;

    std::vector<vk::UniqueDescriptorSetLayout> m_DescriptorSetLayoutTable;
};

class VulkanRenderPipeline : public RenderPipeline {
public:
    VulkanRenderPipeline(vk::Device device, vk::RenderPass renderPass, VulkanProgram&& program);

    vk::UniqueDescriptorSet allocateDescriptorSet(uint32_t binding) const;

    [[nodiscard]] vk::Pipeline getPipeline() const;
    [[nodiscard]] vk::PipelineLayout getPipelineLayout() const { return *m_PipelineLayout; }
    [[nodiscard]] const std::vector<vk::UniqueDescriptorSetLayout>& getDescriptorSetLayoutTable() const { return m_DescriptorSetLayouts; }
private:
    vk::UniquePipelineLayout m_PipelineLayout;
    vk::UniquePipeline m_Pipeline;

    std::vector<vk::UniqueDescriptorSetLayout> m_DescriptorSetLayouts;

    std::vector<vk::DescriptorPoolSize> m_PoolSizes;
    std::vector<vk::UniqueDescriptorPool> m_DescriptorPools;
};

}   // namespace engine::vulkan::render

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_VULKAN_PIPELINE_HPP_
