#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_ELEMENT_ELEMENT_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_ELEMENT_ELEMENT_HPP_

#include "engine/render/render_context.hpp"
#include "engine/render/command/command_buffer.hpp"

namespace engine::gui {

class Element {
public:
    virtual ~Element() = default;

    virtual void draw(engine::render::command::IDrawableCommandBuffer& buffer) = 0;

    virtual void onClick(input::MouseButton button, input::MouseButtonAction action, float x, float y) {}
    virtual bool isInside(float x, float y) { return false; }

    virtual void onResize(uint32_t width, uint32_t height) {}
};

class ElementImage : public Element {
public:
    struct ImageRegion {
        float x, y;
        float width, height;
    };

    struct Options {
        struct {
            float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
        } color;

        uint32_t viewportAdjust = true;
    };

    ElementImage(engine::render::RenderContext& context,
                 std::shared_ptr<engine::render::buffer::Image> image,
                 float x,
                 float y,
                 float w,
                 float h,
                 const ImageRegion& region = {0.0f, 0.0f, 0.0f, 0.0f},
                 const Options& options = Options{1.0f, 1.0f, 1.0f, 1.0f});

    void draw(engine::render::command::IDrawableCommandBuffer& buffer) override;
private:
    std::shared_ptr<render::RenderPipeline> m_Pipeline;
    std::unique_ptr<render::UniformDescriptor> m_UniformDescriptorSet;
    std::shared_ptr<render::buffer::Image> m_Image;
    std::unique_ptr<render::buffer::VertexBuffer> m_VertexBuffer;
    std::unique_ptr<render::buffer::IndexBuffer> m_IndexBuffer;
    Options m_Options;
};

class ElementButton : public Element {
public:
    using ButtonCallback = std::function<void()>;

    ElementButton(engine::render::RenderContext& context, float x, float y, float w, float h);

    void draw(engine::render::command::IDrawableCommandBuffer& buffer) override;

    void onClick(input::MouseButton button, input::MouseButtonAction action, float x, float y) override;
    bool isInside(float x, float y) override;

    void setButtonCallback(ButtonCallback callback) {m_Callback = std::move(callback);}
    void setText(std::u16string text) {m_Text = std::move(text);}

    [[nodiscard]] float getX() const { return m_Position.x; }
    [[nodiscard]] float getY() const { return m_Position.y; }
private:
    engine::render::RenderContext* m_Context;
    std::shared_ptr<render::RenderPipeline> m_Pipeline;
    std::unique_ptr<render::UniformDescriptor> m_UniformDescriptorSet;
    std::shared_ptr<render::buffer::Image> m_Image;
    std::unique_ptr<render::buffer::VertexBuffer> m_VertexBuffer;
    std::unique_ptr<render::buffer::IndexBuffer> m_IndexBuffer;

    struct {
        float x, y;
        float w, h;
    } m_Position;

    ButtonCallback m_Callback;
    std::u16string m_Text;

    struct TexturePositionData{
        struct Vertex{
            float x, y;
            float padding[2];
        };

        Vertex standard[4];
        Vertex hover[4];
    } m_TextureData{};
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_ELEMENT_ELEMENT_HPP_
