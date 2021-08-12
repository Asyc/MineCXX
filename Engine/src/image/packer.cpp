#include "engine/image/packer.hpp"

#include <finders_interface.h>

#include <functional>

#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "engine/render/render_context.hpp"


namespace engine::image {

TexturePacker::TexturePacker(render::RenderContext& context) : m_Context(&context), m_Textures() {}

TexturePacker::~TexturePacker() {
  for (const auto& texture : m_Textures) {
    stbi_image_free(texture.buffer);
  }
}

TexturePacker::Texture* TexturePacker::addImage(const File& path) {
  int width, height, channels;
  auto* buffer = stbi_load(path.getPath().c_str(), &width, &height, &channels, STBI_rgb_alpha);
  return &m_Textures.emplace_back(TexturePacker::Texture{buffer, 0, 0, (uint32_t) width, (uint32_t) height});
}

std::shared_ptr<render::buffer::Image> TexturePacker::stitch() {
  using spaces_type = rectpack2D::empty_spaces<false, rectpack2D::default_empty_spaces>;
  using rect_type = rectpack2D::output_rect_t<spaces_type>;

  auto success = [](rect_type&) {
    return rectpack2D::callback_result::CONTINUE_PACKING;
  };

  auto fail = [](rect_type&) {
    return rectpack2D::callback_result::ABORT_PACKING;
  };

  const auto max_side = 1000;
  const auto discard_step = -4;

  size_t index = 0;
  std::vector<rect_type> rectangles(m_Textures.size());
  for (auto& texture : m_Textures) {
    rectangles[index++] = rect_type{(int) texture.x, (int) texture.y, (int) texture.width, (int) texture.height};
  }

  const auto result_size = rectpack2D::find_best_packing<spaces_type>(rectangles, rectpack2D::make_finder_input(max_side, discard_step, success, fail, rectpack2D::flipping_option::DISABLED));

  struct Component {
    uint8_t r, g, b, a;
  };

  auto out = std::make_unique<Component[]>(result_size.w * result_size.h);
  const auto outRow = result_size.w;

  index = 0;
  for (auto& texture : m_Textures) {
    const auto& rect = rectangles[index++];
    texture = Texture{texture.buffer, (uint32_t) rect.x, (uint32_t) rect.y, (uint32_t) rect.w, (uint32_t) rect.h};

    auto* src = (Component*) texture.buffer;

    const auto row = rect.w;

    for (size_t x = 0; x < rect.w; x++) {
      size_t adjustedX = rect.x + x;

      for (size_t y = 0; y < rect.h; y++) {
        size_t adjustedY = rect.y + y;
        out[adjustedX + adjustedY * outRow] = src[x + y * row];
      }
    }
  }

  auto image = m_Context->createImage(out.get(), (uint32_t) result_size.w, (uint32_t) result_size.h, render::buffer::Image::SamplerOptions());
  return std::move(image);
}

}  // namespace engine::image
