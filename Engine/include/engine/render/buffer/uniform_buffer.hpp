#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_BUFFER_UNIFORM_BUFFER_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_BUFFER_UNIFORM_BUFFER_HPP_

#include <cstdint>

namespace engine::render::buffer {

class UniformBuffer {
public:
    virtual ~UniformBuffer() = default;

    virtual void write(size_t offset, const void* ptr, size_t length) = 0;
protected:
    UniformBuffer() = default;
};

class UniformDescriptor {
public:
    virtual ~UniformDescriptor() = default;

    virtual void bind(uint32_t binding, const UniformBuffer& buffer) = 0;
    virtual void bindAll(uint32_t* bindings, const UniformBuffer* buffers, size_t count) = 0;
protected:
    UniformDescriptor() = default;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_BUFFER_UNIFORM_BUFFER_HPP_
