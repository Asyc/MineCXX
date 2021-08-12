#include "engine/render/camera.hpp"

#include "engine/render/render_context.hpp"
#include "engine/window.hpp"

namespace engine::render {

struct UniformBufferData {
  glm::mat4 projectionMatrix;
  glm::mat4 viewMatrix;
};

Camera::Camera(RenderContext& context) : m_Context(&context), m_Enabled(false), m_Yaw(), m_Pitch(), m_LastYaw(), m_LastPitch() {
  auto [w, h] = context.getSwapchain().getSize();
  float aspect = (float) w / (float) h;

  m_ProjectionMatrix = glm::perspective(glm::radians(90.0f), aspect, 0.0f, 1.0f);

  m_Buffer = context.allocateUniformBuffer(sizeof(UniformBufferData));

  UniformBufferData data{m_ProjectionMatrix, glm::mat4x4(1.0f)};
  m_Buffer->write(0, &data, sizeof(data));

  m_Context->addMousePositionCallback([this](double x, double y) { this->onMouseMove(x, y); });
}

#include <chrono>

void Camera::onMouseMove(double x, double y) {
  using namespace std::chrono_literals;

  static auto lastTick = std::chrono::high_resolution_clock::now();
  static float partialTicks;

  auto now = std::chrono::high_resolution_clock::now();
  if (now - lastTick >= 50ms) {
    lastTick = now;
    partialTicks = 0.0f;
  } else {
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTick).count();
    float seconds = static_cast<float>(millis) / 1000.0f;
    partialTicks = seconds / 20.0f;
  }

  if (!m_Enabled) {
    m_LastX = x;
    m_LastY = y;
    return;
  }

  double deltaX = m_LastX - x;
  double deltaY = m_LastY - y;

  constexpr float sensitivity = 0.5f;
  double factor = std::pow(sensitivity, 3) * 8.0f;
  double yaw = deltaX * factor;
  double pitch = deltaY * factor;

  double lastPitch = m_Pitch;
  double lastYaw = m_Yaw;

  m_Yaw = static_cast<float>(m_Yaw + yaw * 0.15);
  m_Pitch = static_cast<float>(m_Pitch - pitch * 0.15);
  m_Pitch = std::clamp(m_Pitch, -90.0f, 90.0f);

  m_LastPitch += static_cast<float>(m_Pitch - lastPitch);
  m_LastYaw += static_cast<float>(m_Yaw - lastYaw);

  glm::mat4 matrix(1.0f);
  matrix = glm::translate(matrix, glm::vec3(0.0f, 0.0f, -3.6f));
  matrix = glm::rotate(matrix, glm::radians(m_LastPitch + (m_Pitch - m_LastPitch) * partialTicks), glm::vec3(1.0f, 0.0f, 0.0f));
  matrix = glm::rotate(matrix, glm::radians(m_LastYaw + (m_Yaw - m_LastYaw) * partialTicks + 180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  matrix =  glm::inverse(matrix);

  m_LastX = x;
  m_LastY = y;

  UniformBufferData data{m_ProjectionMatrix, matrix};
  m_Buffer->write(0, &data, sizeof(data));
  LOG_INFO("Pitch: {}", m_Pitch);
}

void Camera::setEnabled(bool enabled) {
  m_Context->getWindow().setCursorDisabledFlag(enabled);

  m_Enabled = enabled;
}

const glm::mat4& Camera::getProjectionMatrix() {
  return m_ProjectionMatrix;
}

const glm::mat4& Camera::getViewMatrix() {
  return m_ProjectionMatrix;
}

}