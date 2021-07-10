#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_BUFFER_INDEX_BUFFER_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_BUFFER_INDEX_BUFFER_HPP_

#include <cstdint>

namespace engine::render::buffer {

class IndexBuffer {
public:
    virtual ~IndexBuffer() = default;

    virtual void write(size_t offset, const uint32_t* ptr, size_t length) = 0;
protected:
    IndexBuffer() = default;
};


}   // namespace engine::render::buffer

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_BUFFER_INDEX_BUFFER_HPP_
