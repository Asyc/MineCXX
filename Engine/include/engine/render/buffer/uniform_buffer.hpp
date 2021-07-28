#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_BUFFER_UNIFORM_BUFFER_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_BUFFER_UNIFORM_BUFFER_HPP_

#include <cstdint>

#include "engine/render/descriptor.hpp"

namespace engine::render::buffer {

class UniformBuffer : public IDescriptorResource {
public:
    virtual ~UniformBuffer() = default;

    virtual void write(size_t offset, const void* ptr, size_t length) = 0;
protected:
    UniformBuffer() = default;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_BUFFER_UNIFORM_BUFFER_HPP_
