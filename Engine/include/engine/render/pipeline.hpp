#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_PIPELINE_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_PIPELINE_HPP_

#include <string>

#include "engine/file.hpp"

#include "engine/log.hpp"

namespace engine::render {

class Program {
public:
    virtual ~Program() = default;

    struct ProgramConfig {
        struct VulkanConfig{
            struct InputConfig {
                std::string topology;

                struct Buffer {
                    uint32_t binding;
                    uint32_t stride;
                };

                struct Attribute {
                    uint32_t binding;
                    uint32_t location;
                    std::string type;
                    uint32_t offset;
                };

                std::vector<Buffer> bufferAttributes;
                std::vector<Attribute> inputAttributes;
            } input;

            std::string vertexPath;

            std::string fragmentPath;
        } vulkan;
    } programConfig;

    explicit Program(const File& pipelineConfiguration);
};

class RenderPipeline {
public:
    virtual ~RenderPipeline() = default;
protected:
    RenderPipeline() = default;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_PIPELINE_HPP_
