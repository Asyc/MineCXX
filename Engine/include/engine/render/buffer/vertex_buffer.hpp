#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_BUFFER_VERTEX_BUFFER_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_BUFFER_VERTEX_BUFFER_HPP_

#include <cstdint>

namespace engine::render::buffer {

class VertexBuffer {
public:
    virtual ~VertexBuffer() = default;

    virtual void write(size_t offset, void* ptr, size_t length) = 0;
protected:
    VertexBuffer() = default;


};

}   // namespace engine::render::buffer

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_BUFFER_VERTEX_BUFFER_HPP_
