#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_IMAGE_PACKER_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_IMAGE_PACKER_HPP_

#include <cstdint>
#include <deque>

#include "engine/file.hpp"
#include "engine/render/buffer/image.hpp"

namespace engine::render {
class RenderContext;
}

namespace engine::image {

class TexturePacker {
 public:
  struct Texture {
    unsigned char* buffer;
    uint32_t x, y;
    uint32_t width, height;
  };

  TexturePacker(render::RenderContext& context);

  Texture* addImage(const File& path);

  // Creates an atlas containing all the specified images.
  std::shared_ptr<render::buffer::Image> stitch();
 private:
  render::RenderContext* m_Context;
  std::vector<Texture> m_Textures;
};

}

#endif // MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_IMAGE_PACKER_HPP_