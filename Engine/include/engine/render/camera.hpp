#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_CAMERA_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_CAMERA_HPP_

#include <memory>

#include "engine/math.hpp"
#include "engine/render/buffer/uniform_buffer.hpp"

namespace engine::render {

class RenderContext;

class Camera {
 public:
  Camera(RenderContext& context);

  void setEnabled(bool enabled);

  void onMouseMove(double x, double y);

  const glm::mat4& getProjectionMatrix();
  const glm::mat4& getViewMatrix();

  const std::shared_ptr<buffer::UniformBuffer>& getBuffer() const { return m_Buffer; }
 private:
  RenderContext* m_Context;

  bool m_Enabled;

  std::shared_ptr<buffer::UniformBuffer> m_Buffer;

  glm::mat4 m_ProjectionMatrix;
  float m_Fov;

  double m_LastX, m_LastY;

  float m_Yaw, m_Pitch;
  float m_LastYaw, m_LastPitch;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_CAMERA_HPP_
