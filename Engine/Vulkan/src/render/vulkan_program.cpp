#include "engine/vulkan/render/vulkan_pipeline.hpp"

#include <set>
#include <map>
#include <optional>

#include <glslang/Include/glslang_c_interface.h>
#include <spirv_cross.hpp>
#include <spirv_glsl.hpp>

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

namespace engine::vulkan::render {

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
    case spv::ExecutionModelGeometry: {
      flag = vk::ShaderStageFlagBits::eGeometry;
      break;
    }
    default: throw std::runtime_error("invalid shader stage");
  }

  for (const auto& it : resources.uniform_buffers) {
    const auto& type = compiler.get_type(it.base_type_id);

    uint32_t set = compiler.get_decoration(it.id, spv::DecorationDescriptorSet);
    uint32_t binding = compiler.get_decoration(it.id, spv::DecorationBinding);
    uint32_t array = type.array.empty() ? 1 : type.array[0];

    if (descriptorSetLayouts.size() <= set) {
      descriptorSetLayouts.resize(set + 1);
    }

    descriptorSetLayouts[set].emplace_back(binding, vk::DescriptorType::eUniformBuffer, array, flag, nullptr);
  }

  for (const auto& it : resources.sampled_images) {
    const auto& type = compiler.get_type(it.base_type_id);

    uint32_t set = compiler.get_decoration(it.id, spv::DecorationDescriptorSet);
    uint32_t binding = compiler.get_decoration(it.id, spv::DecorationBinding);
    uint32_t array = type.array.empty() ? 1 : type.array[0];

    if (descriptorSetLayouts.size() <= set) {
      descriptorSetLayouts.resize(set + 1);
    }

    descriptorSetLayouts[set].emplace_back(binding, vk::DescriptorType::eCombinedImageSampler, array, flag, nullptr);
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

  if (lowestAccessed != -1 && highestAccessed != -1) {
    pushConstantRangesOut.emplace_back(flag, lowestAccessed, highestAccessed);
  }
}

template<glslang_stage_t STAGE>
std::vector<char> createSPIRV(std::string src) {
  glslang_resource_s resource{};
  resource.max_draw_buffers = 1;
  resource.max_geometry_output_vertices = 256;
  resource.max_geometry_total_output_components = 1024;
  resource.max_geometry_output_vertices = 256;
  glslang_limits_s limits{
      true, true, true, true, true, true, true, true, true
  };

  resource.limits = limits;

  const glslang_input_t input{
      GLSLANG_SOURCE_GLSL,
      STAGE,
      GLSLANG_CLIENT_VULKAN,
      GLSLANG_TARGET_VULKAN_1_2,
      GLSLANG_TARGET_SPV,
      GLSLANG_TARGET_SPV_1_5,
      src.data(),
      100,
      GLSLANG_CORE_PROFILE,
      false,
      false,
      GLSLANG_MSG_DEFAULT_BIT,
      &resource
  };

  glslang_initialize_process();
  auto* shader = glslang_shader_create(&input);

  const char* shaderStage;
  switch (STAGE) {
    case GLSLANG_STAGE_GEOMETRY:shaderStage = "Geometry Shader";
      break;
    case GLSLANG_STAGE_VERTEX:shaderStage = "Vertex Shader";
      break;
    case GLSLANG_STAGE_FRAGMENT:shaderStage = "Fragment Shader";
      break;
    default:shaderStage = "Unknown Shader";
      break;
  }

  if (!glslang_shader_preprocess(shader, &input)) {
    LOG_ERROR("{}\n{}\n{}\n", shaderStage, glslang_shader_get_info_log(shader), glslang_shader_get_info_debug_log(shader));
    return {};
  }

  if (!glslang_shader_parse(shader, &input)) {
    LOG_ERROR("{}\n{}\n{}\n", shaderStage, glslang_shader_get_info_log(shader), glslang_shader_get_info_debug_log(shader));
    return {};
  }

  glslang_program_t* program = glslang_program_create();
  glslang_program_add_shader(program, shader);

  if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT)) {
    LOG_ERROR("Program:{}\n{}\n{}\n", shaderStage, glslang_shader_get_info_log(shader), glslang_shader_get_info_debug_log(shader));
    return {};
  }

  glslang_program_SPIRV_generate(program, input.stage);

  if (glslang_program_SPIRV_get_messages(program)) {
    printf("%s", glslang_program_SPIRV_get_messages(program));
  }

  glslang_shader_delete(shader);

  auto* ptr = glslang_program_SPIRV_get_ptr(program);
  size_t size = glslang_program_SPIRV_get_size(program);

  std::vector<char> buffer(size * sizeof(unsigned int));
  memcpy(buffer.data(), ptr, buffer.size());

  glslang_program_delete(program);
  return std::move(buffer);
}

VulkanProgram::VulkanProgram(vk::Device device, const std::string_view& program) : Program(File(std::string(program) + "/config.json")) {
  for (const auto& inputBuffer : programConfig.vulkan.input.bufferAttributes) {
    m_InputBindings.emplace_back(inputBuffer.binding, inputBuffer.stride, vk::VertexInputRate::eVertex);    // TODO : Custom input rate
  }

  for (const auto& inputAttribute : programConfig.vulkan.input.inputAttributes) {
    m_InputAttributes.emplace_back(inputAttribute.location, inputAttribute.binding, mapType(inputAttribute.type), inputAttribute.offset);
  }

  const bool hasGeometry = !programConfig.vulkan.geometryPath.empty();

  File vertexFile(programConfig.vulkan.vertexPath);
  File geometryFile(programConfig.vulkan.geometryPath);
  File fragmentFile(programConfig.vulkan.fragmentPath);

#ifdef MCE_DBG
  File vertexLive(programConfig.vulkan.vertexPath.substr(0, programConfig.vulkan.vertexPath.size() - 4));
  File fragmentLive(programConfig.vulkan.fragmentPath.substr(0, programConfig.vulkan.fragmentPath.size() - 4));

  auto vertexSPV = vertexLive.exists() ? createSPIRV<GLSLANG_STAGE_VERTEX>(vertexLive.readFileText()) : vertexFile.readFileBinary();
  auto fragmentSPV = fragmentLive.exists() ? createSPIRV<GLSLANG_STAGE_FRAGMENT>(fragmentLive.readFileText()) : fragmentFile.readFileBinary();
  std::vector<char> geometrySPV;

  vertexFile.write(vertexSPV.data(), vertexSPV.size());
  fragmentFile.write(fragmentSPV.data(), fragmentSPV.size());

  if (hasGeometry) {
    File geometryLive(programConfig.vulkan.geometryPath.substr(0, programConfig.vulkan.geometryPath.size() - 4));
    geometrySPV = geometryLive.exists() ? createSPIRV<GLSLANG_STAGE_GEOMETRY>(geometryLive.readFileText()) : geometryFile.readFileBinary();
    geometryFile.write(geometrySPV.data(), geometrySPV.size());
  }
#else
  auto vertexSPV = vertexFile.readFileBinary();
  auto fragmentSPV = fragmentFile.readFileBinary();

  std::vector<char> geometrySPV;
  if (hasGeometry) geometrySPV = geometryFile.readFileBinary();
#endif

  vk::ShaderModuleCreateInfo vertexModuleCreateInfo({}, vertexSPV.size(), reinterpret_cast<const uint32_t*>(vertexSPV.data()));
  m_Vertex = device.createShaderModuleUnique(vertexModuleCreateInfo);

  vk::ShaderModuleCreateInfo fragmentModuleCreateInfo({}, fragmentSPV.size(), reinterpret_cast<const uint32_t*>(fragmentSPV.data()));
  m_Fragment = device.createShaderModuleUnique(fragmentModuleCreateInfo);

  vk::ShaderModuleCreateInfo geometryModuleCreateInfo({}, geometrySPV.size(), reinterpret_cast<const uint32_t*>(geometrySPV.data()));
  if (hasGeometry) m_Geometry = device.createShaderModuleUnique(geometryModuleCreateInfo);

  m_Stages.emplace_back(vk::PipelineShaderStageCreateFlagBits{}, vk::ShaderStageFlagBits::eVertex, *m_Vertex, "main");
  m_Stages.emplace_back(vk::PipelineShaderStageCreateFlagBits{}, vk::ShaderStageFlagBits::eFragment, *m_Fragment, "main");
  if (hasGeometry) m_Stages.emplace_back(vk::PipelineShaderStageCreateFlagBits{}, vk::ShaderStageFlagBits::eGeometry, *m_Geometry, "main");

  std::vector<std::vector<vk::DescriptorSetLayoutBinding>> setBindingTable;
  std::vector<vk::PushConstantRange> pushConstantRanges;
  parseShader(vertexSPV, setBindingTable, pushConstantRanges);
  parseShader(fragmentSPV, setBindingTable, pushConstantRanges);
  if (hasGeometry) parseShader(geometrySPV, setBindingTable, pushConstantRanges);

  size_t setIndex = 0;
  for (auto& bindings : setBindingTable) {
    uint32_t maxBinding = -1;

    for (const auto& bindingData : bindings) {
      if (maxBinding == -1 || bindingData.binding > maxBinding) maxBinding = bindingData.binding;
    }

    std::vector<vk::DescriptorSetLayoutBinding> flattenedBindings(maxBinding + 1);
    std::set<uint32_t> indexSet;

    for (const auto& bindingData : bindings) {
      if (indexSet.find(bindingData.binding) != indexSet.end()) {
        flattenedBindings[bindingData.binding].stageFlags |= bindingData.stageFlags;
        continue;
      }

      flattenedBindings[bindingData.binding] = bindingData;
      indexSet.insert(bindingData.binding);
    }

    bindings = std::move(flattenedBindings);
    setIndex++;
  }

  std::vector<vk::UniqueDescriptorSetLayout> descriptorSets(setBindingTable.size());
  uint32_t index = 0;
  for (const auto& bindings : setBindingTable) {
    vk::DescriptorSetLayoutCreateInfo createInfo({}, bindings.size(), bindings.data());
    descriptorSets[index++] = device.createDescriptorSetLayoutUnique(createInfo);
  }

  auto* buffer = (vk::DescriptorSetLayout*) alloca(sizeof(vk::DescriptorSet) * descriptorSets.size());
  for (size_t i = 0; i < descriptorSets.size(); i++) buffer[i] = *descriptorSets[i];

  vk::PipelineLayoutCreateInfo layoutCreateInfo({}, descriptorSets.size(), buffer, pushConstantRanges.size(), pushConstantRanges.data());
  m_PipelineLayout = device.createPipelineLayoutUnique(layoutCreateInfo);
  m_DescriptorSetLayoutTable = std::move(descriptorSets);
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

}   // namespace engine::vulkan::render