#ifndef MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_VULKAN_PIPELINE_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_VULKAN_PIPELINE_HPP_

#include "engine/render/pipeline.hpp"

#include <memory>
#include <string_view>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "engine/file.hpp"

namespace engine::render::vulkan {

class VulkanProgram : public Program {
public:
    VulkanProgram(vk::Device device, const std::string_view& program);

    [[nodiscard]] const std::vector<vk::PipelineShaderStageCreateInfo>& getStages() const;
    [[nodiscard]] vk::PipelineLayout getPipelineLayout() const;

    [[nodiscard]] const std::vector<vk::VertexInputBindingDescription>& getInputBindings() const;
    [[nodiscard]] const std::vector<vk::VertexInputAttributeDescription>& getInputAttributes() const;

private:
    vk::UniqueShaderModule m_Vertex, m_Fragment;
    std::vector<vk::PipelineShaderStageCreateInfo> m_Stages;
    vk::UniquePipelineLayout m_PipelineLayout;

    std::vector<vk::VertexInputBindingDescription> m_InputBindings;
    std::vector<vk::VertexInputAttributeDescription> m_InputAttributes;
};

class VulkanRenderPipeline : public RenderPipeline {
public:
    VulkanRenderPipeline(vk::Device device, vk::RenderPass renderPass, std::shared_ptr<VulkanProgram> program);

    [[nodiscard]] vk::Pipeline getPipeline() const;
private:

    std::shared_ptr<VulkanProgram> m_Program;

    vk::UniquePipeline m_Pipeline;
};

}   // namespace engine::render::vulkan

#endif //MINECRAFTCXX_CLIENT_ENGINE_VULKAN_INCLUDE_ENGINE_VULKAN_RENDER_VULKAN_PIPELINE_HPP_
