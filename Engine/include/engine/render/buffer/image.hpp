#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_IMAGE_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_IMAGE_HPP_

namespace engine::render::buffer {

class Image {
public:
    virtual ~Image() = default;
protected:
    Image() = default;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_IMAGE_HPP_
