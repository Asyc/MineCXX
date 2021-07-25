#include "engine/render/pipeline.hpp"

#include <rapidjson/document.h>

#include "engine/log.hpp"

namespace engine::render {

template <typename Document>
inline std::string getString(const Document& document, const char* key) {
    const auto& element = document[key];
    return std::string(element.GetString(), element.GetStringLength());
}

template <typename Document>
inline std::string getStringOptional(const Document& document, const char* key, const char* defaultValue) {
    if (document.HasMember(key)) {
        return getString(document, key);
    }

    return {defaultValue};
}

Program::Program(const File& pipelineConfiguration) {
    rapidjson::Document document;
    auto json = pipelineConfiguration.readFileText();
    document.Parse(json.c_str(), json.size());

    auto vulkan = document["vulkan"].GetObject();
    programConfig.vulkan.vertexPath = pipelineConfiguration.getParentPath() + '/' + getStringOptional(vulkan, "vertexPath", "vertex_shader.spv");

    std::string geometryPath = getStringOptional(vulkan, "geometryPath", "");
    if (!geometryPath.empty()) programConfig.vulkan.geometryPath = pipelineConfiguration.getParentPath() + '/' + geometryPath;

    programConfig.vulkan.fragmentPath = pipelineConfiguration.getParentPath() + '/' + getStringOptional(vulkan, "fragmentPath", "fragment_shader.spv");

    auto vulkanInput = vulkan["input"].GetObject();
    auto vulkanInputBuffers = vulkanInput["buffers"].GetArray();

    programConfig.vulkan.input.topology = getString(vulkanInput, "topology");

    for (const auto& inputBuffer : vulkanInputBuffers) {
        auto object = inputBuffer.GetObject();
        programConfig.vulkan.input.bufferAttributes.push_back({object["binding"].GetUint(), object["stride"].GetUint()});
    }

    auto vulkanInputAttributes = vulkanInput["attributes"].GetArray();

    for (const auto& inputAttribute : vulkanInputAttributes) {
        auto object = inputAttribute.GetObject();
        programConfig.vulkan.input.inputAttributes.push_back({
            object["binding"].GetUint(),
            object["location"].GetUint(),
            getString(object, "type"),
            object["offset"].GetUint()
        });
    }

    MCE_LOG_DEBUG("Registered pipeline: {}, Config: Custom", pipelineConfiguration.getPath());
}

}   // namespace engine::render