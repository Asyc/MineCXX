#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_SCENE_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_SCENE_HPP_

#include <memory>

#include "gui.hpp"
#include "input.hpp"

#include "engine/render/render_context.hpp"

namespace engine::gui {

class Scene {
 public:
  explicit Scene(render::RenderContext& context);

  void setGui(std::unique_ptr<Gui> gui) { m_Gui = std::move(gui); }

  template<typename T>
  typename std::enable_if<std::is_base_of<Gui, T>::value>::type setGui() { setGui(std::make_unique<T>(*m_Context, *this)); }

  void render();
  void update();

  void onResize(uint32_t width, uint32_t height);
  void onMouseAction(input::MouseButton button, input::MouseButtonAction action);
 private:
  render::RenderContext* m_Context;

  std::unique_ptr<Gui> m_Gui;
  std::unique_ptr<render::command::DirectCommandBuffer> m_CommandBuffer;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_SCENE_HPP_
