#include "engine/vulkan/render/vulkan_pipeline.hpp"

#include <spirv_cross.hpp>
#include <spirv_cpp.hpp>
#include <spirv_glsl.hpp>

#include <vulkan/vulkan.hpp>

#include <map>
#include <optional>

#include "engine/log.hpp"

namespace engine::render::vulkan {

VulkanRenderPipeline::VulkanRenderPipeline(vk::Device device, std::shared_ptr<VulkanProgram> program) : m_Program(std::move(program)) {
    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo(
        {},
        m_Program->getInputBindings().size(),
        m_Program->getInputBindings().data(),
        m_Program->getInputAttributes().size(),
        m_Program->getInputAttributes().data()
    );

    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo({}, vk::PrimitiveTopology::eTriangleList, VK_FALSE);

    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo;

    vk::GraphicsPipelineCreateInfo pipelineCreateInfo(
        {},
        m_Program->getStages().size(),
        m_Program->getStages().data(),
        &vertexInputStateCreateInfo,
        &inputAssemblyStateCreateInfo,
        nullptr
    );
}

vk::Pipeline VulkanRenderPipeline::getPipeline() const {
    return {};
}

}   // namespace engine::render::vulkan