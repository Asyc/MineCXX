#include "engine/gui/scene.hpp"

#include "engine/gui/gui.hpp"
#include "engine/render/viewport.hpp"

namespace engine::gui {

Scene::Scene(render::RenderContext& context) : m_Context(&context), m_Gui() {
  m_Context->addMouseCallback([this](input::MouseButton button, input::MouseButtonAction action) { this->onMouseAction(button, action); });
  m_CommandBuffer = context.getThreadCommandPool().allocateDirectCommandBuffer();
}

void Scene::render() {
  if (m_Gui != nullptr || m_World != nullptr) {
    m_Context->getSwapchain().submitCommandBuffer(*m_CommandBuffer);
  }
}

void Scene::update() {
  auto now = std::chrono::high_resolution_clock::now();
  bool tick = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_LastTick).count() >= 50;
  if (tick) m_LastTick = now;

  if (m_World != nullptr) {
    if (tick) m_World->tick();
    m_World->update();
  }

  m_CommandBuffer->begin();
  if (m_Gui != nullptr) m_Gui->draw(*m_CommandBuffer);
  if (m_World != nullptr) m_World->draw(*m_CommandBuffer);
  m_CommandBuffer->end();
}

void Scene::onResize(uint32_t width, uint32_t height) {
  update();
}

void Scene::onMouseAction(input::MouseButton button, input::MouseButtonAction action) {
  if (m_Gui != nullptr) {
    auto[mouseX, mouseY] = m_Context->getViewport().getMousePosition();
    m_Gui->onClick(button, action, mouseX, mouseY);
  }
}

}