#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_IMAGE_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_IMAGE_HPP_

#include <cstdint>
#include <tuple>

#include "engine/render/descriptor.hpp"

namespace engine::render::buffer {

class Image : public IDescriptorResource {
public:
    enum class Filter {
        LINEAR,
        NEAREST
    };

    enum class RepeatMode {
        REPEAT,
        CLAMP_TO_BORDER,
        CLAMP_TO_EDGE
    };

    struct SamplerOptions {
        Filter minFilter, magFilter;
        RepeatMode repeatX, repeatY;

        SamplerOptions(Filter minFilter, Filter magFilter, RepeatMode repeatX, RepeatMode repeatY) : minFilter(minFilter), magFilter(magFilter), repeatX(repeatX), repeatY(repeatY) {}
        SamplerOptions() : SamplerOptions(Filter::NEAREST, Filter::NEAREST, RepeatMode::REPEAT, RepeatMode::REPEAT) {}


    };

    ~Image() override = default;

    [[nodiscard]] virtual size_t getWidth() const = 0;
    [[nodiscard]] virtual size_t getHeight() const = 0;

    [[nodiscard]] std::pair<size_t, size_t> getSize() const { return {getWidth(), getHeight()}; }
protected:
    Image() = default;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_IMAGE_HPP_
