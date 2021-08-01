#include "engine/render/viewport.hpp"

#include "engine/window.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "engine/render/render_context.hpp"

namespace engine::render {

ViewportGUI::ViewportGUI(RenderContext& context) : m_Owner(&context) {
    m_Viewport = glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);
    m_Matrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);

    m_ViewportBuffer = std::shared_ptr<buffer::UniformBuffer>(context.allocateUniformBuffer(sizeof(glm::mat4)));
    m_ViewportBuffer->write(0, &m_Matrix, sizeof(glm::mat4));

    m_Owner->setResizeCallback([this](uint32_t width, uint32_t height){
        /*float aspectRatio = 1080.0f / 1920.0f;
        m_Viewport = glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);
        m_Matrix = glm::ortho(-1.0f, 1.0f, -1.0f * aspectRatio, 1.0f * aspectRatio);

        m_ViewportBuffer->write(0, &m_Matrix, sizeof(glm::mat4));*/
        LOG_INFO("Resize Callback: [{}, {}]", width, height);
    });
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

    return {viewportMouseX, viewportMouseY};
}

}