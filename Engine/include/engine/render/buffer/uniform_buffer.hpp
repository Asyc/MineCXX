#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_BUFFER_UNIFORM_BUFFER_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_BUFFER_UNIFORM_BUFFER_HPP_

#include <cstdint>

namespace engine::render::buffer {

class IUniformBuffer {
public:
    virtual ~IUniformBuffer() = default;
protected:
    IUniformBuffer() = default;
};

class UniformBuffer {
public:
    virtual ~UniformBuffer() = default;

    virtual void write(size_t offset, const void* ptr, size_t length) = 0;
protected:
    UniformBuffer() = default;
};

class DirectUniformBuffer {
public:
    virtual ~DirectUniformBuffer() = default;

    virtual void write(size_t offset, const void* ptr, size_t length) = 0;
protected:
    DirectUniformBuffer() = default;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_BUFFER_UNIFORM_BUFFER_HPP_
