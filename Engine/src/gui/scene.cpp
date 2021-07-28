#include "engine/gui/scene.hpp"

#include "engine/gui/gui.hpp"

namespace engine::gui {

Scene::Scene(render::RenderContext& context) : m_Context(&context), m_Gui() {
    m_CommandBuffer = context.getThreadCommandPool().allocateDirectCommandBuffer();
}

void Scene::render() {
    m_Context->getSwapchain().submitCommandBuffer(*m_CommandBuffer);
}

void Scene::update() {
    m_CommandBuffer->begin();
    if (m_Gui != nullptr) m_Gui->draw(*m_CommandBuffer);
    m_CommandBuffer->end();
}

void Scene::onResize(uint32_t width, uint32_t height) {
    update();
}

void Scene::onMouseAction(input::MouseButton button, input::MouseButtonAction action, float x, float y) {

}

}