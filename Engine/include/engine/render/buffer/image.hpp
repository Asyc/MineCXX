#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_IMAGE_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_IMAGE_HPP_

#include <cstdint>

namespace engine::render::buffer {

class Image {
public:
    virtual ~Image() = default;

    [[nodiscard]] virtual size_t getWidth() const = 0;
    [[nodiscard]] virtual size_t getHeight() const = 0;

    std::pair<size_t, size_t> getSize() { return {getWidth(), getHeight()}; }
protected:
    Image() = default;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_IMAGE_HPP_
