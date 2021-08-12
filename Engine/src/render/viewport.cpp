#include "engine/render/viewport.hpp"

#include "engine/math.hpp"
#include "engine/window.hpp"


#include "engine/render/render_context.hpp"

namespace engine::render {

struct ViewportBuffer {
  glm::mat4x4 projectionMatrix;
  float scaleFactor;
  float _padding[3];
};

ViewportGUI::ViewportGUI(RenderContext& context) : m_Owner(&context) {
  m_ViewportBuffer = context.allocateUniformBuffer(sizeof(ViewportBuffer));

  RenderContext::ResizeCallback callback = [this](uint32_t w, uint32_t h) {
    m_ScaleFactor = 30.0f;

    auto width = static_cast<float>(w);
    auto height = static_cast<float>(h);

    m_Viewport = glm::vec4(-width / m_ScaleFactor, width / m_ScaleFactor, -height / m_ScaleFactor, height / m_ScaleFactor);
    m_Matrix = glm::ortho(-width / m_ScaleFactor, width / m_ScaleFactor, -height / m_ScaleFactor, height / m_ScaleFactor);

    ViewportBuffer buffer{m_Matrix, m_ScaleFactor};
    m_ViewportBuffer->write(0, &buffer, sizeof(ViewportBuffer));
  };

  auto[width, height] = context.getSwapchain().getSize();
  std::invoke(callback, width, height);
  m_Owner->addResizeCallback(std::move(callback));
}

std::pair<float, float> ViewportGUI::getMousePosition() const {
  auto[mouseX, mouseY] = m_Owner->getWindow().getCursorPosition();
  auto[windowWidth, windowHeight] = m_Owner->getSwapchain().getSize();

  float normalizedMouseX = static_cast<float>(mouseX) / static_cast<float>(windowWidth);
  float normalizedMouseY = static_cast<float>(mouseY) / static_cast<float>(windowHeight);

  float viewportX = m_Viewport.x;
  float viewportWidth = m_Viewport.y - m_Viewport.x;

  float viewportY = m_Viewport.a;
  float viewportHeight = m_Viewport.a - m_Viewport.z;

  float viewportMouseX = viewportX + (viewportWidth * normalizedMouseX);
  float viewportMouseY = viewportY - (viewportHeight * normalizedMouseY); // Mouse Coordinates go top-down

  viewportMouseX /= m_ScaleFactor;
  viewportMouseY /= m_ScaleFactor;

  return {viewportMouseX, viewportMouseY};
}

}