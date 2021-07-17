#include "engine/vulkan/render/vulkan_pipeline.hpp"

#include <map>
#include <optional>

namespace engine::vulkan::render {

VulkanRenderPipeline::VulkanRenderPipeline(vk::Device device, vk::RenderPass renderPass, std::shared_ptr<VulkanProgram> program) : m_Program(std::move(program)) {
    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo(
        {},
        m_Program->getInputBindings().size(),
        m_Program->getInputBindings().data(),
        m_Program->getInputAttributes().size(),
        m_Program->getInputAttributes().data()
    );

    vk::PrimitiveTopology topology;
    if (m_Program->programConfig.vulkan.input.topology == "ePointList") {
        topology = vk::PrimitiveTopology::ePointList;
    }

    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo({}, vk::PrimitiveTopology::eTriangleList, VK_FALSE);

    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo({}, 1, nullptr, 1, nullptr);

    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo(
        {},
        VK_FALSE,
        VK_FALSE,
        vk::PolygonMode::eFill,
        vk::CullModeFlagBits::eNone,
        vk::FrontFace::eClockwise,
        VK_FALSE,
        0.0f,
        0.0f,
        0.f,
        1.0f
    );

    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo({}, vk::SampleCountFlagBits::e1, VK_FALSE);

    vk::PipelineColorBlendAttachmentState attachmentState(
        VK_FALSE,
        vk::BlendFactor::eZero,
        vk::BlendFactor::eZero,
        vk::BlendOp::eAdd,
        vk::BlendFactor::eZero,
        vk::BlendFactor::eZero,
        vk::BlendOp::eAdd,
        vk::ColorComponentFlags{vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA}
    );
    vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo({}, {}, vk::LogicOp::eClear, 1, &attachmentState, {});

    std::array<vk::DynamicState, 2> dynamicState = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor
    };

    vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo({}, dynamicState.size(), dynamicState.data());

    vk::GraphicsPipelineCreateInfo pipelineCreateInfo(
        {},
        m_Program->getStages().size(),
        m_Program->getStages().data(),
        &vertexInputStateCreateInfo,
        &inputAssemblyStateCreateInfo,
        nullptr,
        &viewportStateCreateInfo,
        &rasterizationStateCreateInfo,
        &multisampleStateCreateInfo,
        nullptr,
        &colorBlendStateCreateInfo,
        &dynamicStateCreateInfo,
        m_Program->getPipelineLayout(),
        renderPass
    );

    m_Pipeline = device.createGraphicsPipelineUnique({}, pipelineCreateInfo).value;
}

vk::Pipeline VulkanRenderPipeline::getPipeline() const {
    return *m_Pipeline;
}

}   // namespace engine::vulkan::render