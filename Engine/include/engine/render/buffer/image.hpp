#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_IMAGE_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_IMAGE_HPP_

#include <cstdint>
#include <tuple>

#include "engine/render/descriptor.hpp"

namespace engine::render::buffer {

class Sampler {
public:
    enum class Filtering {
        LINEAR,
        NEAREST
    };

    enum class RepeatMode {

    };
};

class Image : public IDescriptorResource {
public:
    virtual ~Image() = default;

    [[nodiscard]] virtual size_t getWidth() const = 0;
    [[nodiscard]] virtual size_t getHeight() const = 0;

    [[nodiscard]] std::pair<size_t, size_t> getSize() const { return {getWidth(), getHeight()}; }
protected:
    Image() = default;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_IMAGE_HPP_
