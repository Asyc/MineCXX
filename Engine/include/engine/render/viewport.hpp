#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_VIEWPORT_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_VIEWPORT_HPP_

#include <tuple>

#include <glm/mat4x4.hpp>

#include "engine/render/render_context.hpp"
#include "engine/render/buffer/uniform_buffer.hpp"

namespace engine::render {

class ViewportGUI {
public:
    explicit ViewportGUI(engine::render::RenderContext& context);

    [[nodiscard]] std::pair<float, float> getMousePosition() const;

    [[nodiscard]] const std::shared_ptr<buffer::UniformBuffer>& getUniformBuffer() const { return m_ViewportBuffer; }
    [[nodiscard]] const glm::mat4& getMatrix() const { return m_Matrix; }
private:
    RenderContext* m_Owner;

    std::shared_ptr<buffer::UniformBuffer> m_ViewportBuffer;

    glm::mat4 m_Matrix;
    glm::vec4 m_Viewport;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_VIEWPORT_HPP_
