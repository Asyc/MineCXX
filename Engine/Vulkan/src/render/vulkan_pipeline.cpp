#include "engine/vulkan/render/vulkan_pipeline.hpp"

#include <map>
#include <optional>

namespace engine::vulkan::render {

VulkanRenderPipeline::VulkanRenderPipeline(vk::Device device, vk::RenderPass renderPass, VulkanProgram&& program) {
    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo(
        {},
        program.getInputBindings().size(),
        program.getInputBindings().data(),
        program.getInputAttributes().size(),
        program.getInputAttributes().data()
    );

    vk::PrimitiveTopology topology;
    if (program.programConfig.vulkan.input.topology == "ePointList") {
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
        program.getStages().size(),
        program.getStages().data(),
        &vertexInputStateCreateInfo,
        &inputAssemblyStateCreateInfo,
        nullptr,
        &viewportStateCreateInfo,
        &rasterizationStateCreateInfo,
        &multisampleStateCreateInfo,
        nullptr,
        &colorBlendStateCreateInfo,
        &dynamicStateCreateInfo,
        program.getPipelineLayout(),
        renderPass
    );

    m_PipelineLayout = std::move(program.m_PipelineLayout);
    m_Pipeline = device.createGraphicsPipelineUnique({}, pipelineCreateInfo).value;

    m_DescriptorSetLayouts = std::move(program.m_DescriptorSetLayoutTable);
    m_DescriptorSetLayouts.shrink_to_fit();

    m_PoolSizes.emplace_back(vk::DescriptorType::eUniformBuffer, static_cast<uint32_t>(m_DescriptorSetLayouts.size()));
    m_DescriptorPools.push_back(device.createDescriptorPoolUnique(vk::DescriptorPoolCreateInfo(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 3, m_PoolSizes.size(), m_PoolSizes.data())));
}

vk::UniqueDescriptorSet VulkanRenderPipeline::allocateDescriptorSet(uint32_t binding) const {
    vk::DescriptorSetAllocateInfo allocateInfo(*m_DescriptorPools[0], 1, &*m_DescriptorSetLayouts[binding]);
    return std::move(m_Pipeline.getOwner().allocateDescriptorSetsUnique(allocateInfo)[0]);
}

vk::Pipeline VulkanRenderPipeline::getPipeline() const {
    return *m_Pipeline;
}

}   // namespace engine::vulkan::render