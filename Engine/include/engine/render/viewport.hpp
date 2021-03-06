#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_VIEWPORT_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_VIEWPORT_HPP_

#include <memory>
#include <tuple>

#include "engine/math.hpp"
#include "engine/render/buffer/uniform_buffer.hpp"

namespace engine::render {

class RenderContext;

class ViewportGUI {
 public:
  explicit ViewportGUI(engine::render::RenderContext& context);

  [[nodiscard]] std::pair<float, float> getMousePosition() const;

  [[nodiscard]] const std::shared_ptr<buffer::UniformBuffer>& getUniformBuffer() const { return m_ViewportBuffer; }
  [[nodiscard]] const glm::mat4& getMatrix() const { return m_Matrix; }

  float getLeft() const { return m_Viewport.x; }
  float getRight() const { return m_Viewport.y; }
  float getBottom() const { return m_Viewport.z; }
  float getTop() const { return m_Viewport.w; }
 private:
  RenderContext* m_Owner;

  std::shared_ptr<buffer::UniformBuffer> m_ViewportBuffer;

  glm::mat4 m_Matrix;
  glm::vec4 m_Viewport;

  float m_ScaleFactor;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_VIEWPORT_HPP_
