#include "engine/vulkan/render/vulkan_pipeline.hpp"

#include <spirv_cross.hpp>
#include <spirv_cpp.hpp>
#include <spirv_glsl.hpp>

#include <vulkan/vulkan.hpp>

#include <map>
#include <optional>

#include "engine/log.hpp"

#define CREATE_TYPE_MAPPING(type, prefix, vkType, bits)                                 \
do {                                                                                    \
    if ((type).rfind((#prefix), 0) == 0) {                                              \
        switch ((type).data()[type.size() - 1] - '0') {                                 \
            case 1:                                                                     \
                return (vk::Format::eR##bits##S##vkType);                               \
            case 2:                                                                     \
                return (vk::Format::eR##bits##G##bits##S##vkType);                      \
            case 3:                                                                     \
                return (vk::Format::eR##bits##G##bits##B##bits##S##vkType);             \
            case 4:                                                                     \
                return (vk::Format::eR##bits##G##bits##B##bits##A##bits##S##vkType);    \
            default:                                                                    \
                throw std::runtime_error("unknown type in shader config");              \
        }                                                                               \
    }                                                                                   \
} while (false)

namespace engine::render::vulkan {

inline vk::Format mapType(const std::string_view& type) {
    CREATE_TYPE_MAPPING(type, byte, int, 8);
    CREATE_TYPE_MAPPING(type, short, int, 16);
    CREATE_TYPE_MAPPING(type, int, int, 32);
    CREATE_TYPE_MAPPING(type, long, int, 64);
    CREATE_TYPE_MAPPING(type, float, float, 32);
    CREATE_TYPE_MAPPING(type, double, float, 64);

    return vk::Format::eUndefined;
}

inline void parseShader(const std::vector<char>& shaderBuffer,
                        std::vector<std::vector<vk::DescriptorSetLayoutBinding>>& descriptorSetLayouts,
                        std::vector<vk::PushConstantRange>& pushConstantRangesOut) {
    spirv_cross::Compiler compiler(reinterpret_cast<const uint32_t*>(shaderBuffer.data()), shaderBuffer.size() / sizeof(uint32_t));
    auto resources = compiler.get_shader_resources();

    vk::ShaderStageFlagBits flag;
    if (compiler.get_entry_points_and_stages().size() != 1) {
        throw std::runtime_error("invalid spir-v shader");
    }

    auto& entryPoint = compiler.get_entry_points_and_stages().front();
    switch (entryPoint.execution_model) {
        case spv::ExecutionModelVertex: {
            flag = vk::ShaderStageFlagBits::eVertex;
            break;
        }
        case spv::ExecutionModelFragment: {
            flag = vk::ShaderStageFlagBits::eFragment;
            break;
        }
        default: throw std::runtime_error("invalid shader stage");
    }

    for (const auto& it : resources.uniform_buffers) {
        const auto& type = compiler.get_type(it.base_type_id);

        uint32_t set = compiler.get_decoration(it.id, spv::DecorationDescriptorSet);
        uint32_t binding = compiler.get_decoration(it.id, spv::DecorationBinding);
        uint32_t array = type.array.empty() ? 1 : type.array[0];

        if (descriptorSetLayouts.size() <= binding) {
            descriptorSetLayouts.resize(binding + 1);
        }

        descriptorSetLayouts[set].emplace_back(binding, vk::DescriptorType::eUniformBuffer, array, flag, nullptr);
    }

    uint32_t lowestAccessed = -1, highestAccessed = -1;


    //Spec states only 1 push constant buffer per stage
    for (const auto& it : resources.push_constant_buffers) {
        const auto& type = compiler.get_type(it.type_id);

        for (uint32_t i = 0; i < type.member_types.size(); i++) {
            uint32_t offset = compiler.get_member_decoration(it.base_type_id, i, spv::DecorationOffset);
            uint32_t memberSize = compiler.get_declared_struct_member_size(type, i);

            if (lowestAccessed == -1 || offset < lowestAccessed) lowestAccessed = offset;
            uint32_t lastIndex = offset + memberSize;
            if (highestAccessed == -1 || lastIndex > highestAccessed) highestAccessed = lastIndex;
        }
    }

    pushConstantRangesOut.emplace_back(flag, lowestAccessed, highestAccessed);
}

VulkanProgram::VulkanProgram(vk::Device device, const std::string_view& program) : Program(File(std::string(program) + "/config.json")) {
    for (const auto& inputBuffer : programConfig.vulkan.input.bufferAttributes) {
        m_InputBindings.emplace_back(inputBuffer.binding, inputBuffer.stride, vk::VertexInputRate::eVertex);    // TODO : Custom input rate
    }

    for (const auto& inputAttribute : programConfig.vulkan.input.inputAttributes) {
        m_InputAttributes.emplace_back(inputAttribute.location, inputAttribute.binding, mapType(inputAttribute.type), inputAttribute.offset);
    }

    auto vertexSPV = File(programConfig.vulkan.vertexPath).readFileBinary();
    auto fragmentSPV = File(programConfig.vulkan.fragmentPath).readFileBinary();

    vk::ShaderModuleCreateInfo vertexModuleCreateInfo({}, vertexSPV.size(), reinterpret_cast<const uint32_t*>(vertexSPV.data()));
    m_Vertex = device.createShaderModuleUnique(vertexModuleCreateInfo);

    vk::ShaderModuleCreateInfo fragmentModuleCreateInfo({}, fragmentSPV.size(), reinterpret_cast<const uint32_t*>(fragmentSPV.data()));
    m_Fragment = device.createShaderModuleUnique(fragmentModuleCreateInfo);

    m_Stages.emplace_back(vk::PipelineShaderStageCreateFlagBits{}, vk::ShaderStageFlagBits::eVertex, *m_Vertex, "main");
    m_Stages.emplace_back(vk::PipelineShaderStageCreateFlagBits{}, vk::ShaderStageFlagBits::eFragment, *m_Fragment, "main");

    spirv_cross::Compiler vertexShader(reinterpret_cast<const uint32_t*>(vertexSPV.data()), vertexSPV.size() / sizeof(uint32_t));
    auto vertexShaderData = vertexShader.get_shader_resources();

    std::vector<std::vector<vk::DescriptorSetLayoutBinding>> setBindingTable;
    std::vector<vk::PushConstantRange> pushConstantRanges;
    parseShader(vertexSPV, setBindingTable, pushConstantRanges);
    parseShader(fragmentSPV, setBindingTable, pushConstantRanges);

    std::vector<vk::DescriptorSetLayout> descriptorSets(setBindingTable.size());
    uint32_t index = 0;
    for (const auto& bindings : setBindingTable) {
        vk::DescriptorSetLayoutCreateInfo createInfo({}, bindings.size(), bindings.data());
        descriptorSets[index++] = device.createDescriptorSetLayout(createInfo);
    }

    vk::PipelineLayoutCreateInfo layoutCreateInfo({}, descriptorSets.size(), descriptorSets.data(), pushConstantRanges.size(), pushConstantRanges.data());
    m_PipelineLayout = device.createPipelineLayoutUnique(layoutCreateInfo);

    std::for_each(descriptorSets.begin(), descriptorSets.end(), [device](vk::DescriptorSetLayout layout) { device.destroyDescriptorSetLayout(layout); });
}

const std::vector<vk::PipelineShaderStageCreateInfo>& VulkanProgram::getStages() const {
    return m_Stages;
}

vk::PipelineLayout VulkanProgram::getPipelineLayout() const {
    return *m_PipelineLayout;
}

const std::vector<vk::VertexInputBindingDescription>& VulkanProgram::getInputBindings() const {
    return m_InputBindings;
}
const std::vector<vk::VertexInputAttributeDescription>& VulkanProgram::getInputAttributes() const {
    return m_InputAttributes;
}

}   // namespace engine::render::vulkan