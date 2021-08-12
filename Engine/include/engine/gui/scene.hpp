#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_SCENE_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_SCENE_HPP_

#include <chrono>
#include <memory>

#include "gui.hpp"
#include "input.hpp"

#include "engine/render/render_context.hpp"
#include "engine/world_object.hpp"

namespace engine::gui {

class Scene {
 public:
  explicit Scene(render::RenderContext& context);

  void setGui(std::unique_ptr<Gui> gui) {
    m_Gui = std::move(gui);
  }

  void setGui(std::nullptr_t) {
    if (m_Gui != nullptr) {
      m_Context->getSwapchain().addResourceFree(std::shared_ptr<Gui>(std::move(m_Gui)));
      m_Gui = nullptr;
    }
  }
  template<typename T>
  typename std::enable_if<std::is_base_of<Gui, T>::value>::type setGui() { setGui(std::make_unique<T>(*m_Context, *this)); }

  void setWorldObject(std::shared_ptr<WorldObject> world) { m_World = std::move(world); m_Context->getCamera().setEnabled(true); }

  void render();
  void update();

  void onResize(uint32_t width, uint32_t height);
  void onMouseAction(input::MouseButton button, input::MouseButtonAction action);
  void onMouseMove(double x, double y);
 private:
  render::RenderContext* m_Context;

  double m_MouseX, m_MouseY;

  std::unique_ptr<Gui> m_Gui;
  std::unique_ptr<render::command::DirectCommandBuffer> m_CommandBuffer;

  std::shared_ptr<WorldObject> m_World;

  std::chrono::high_resolution_clock::time_point m_LastTick;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_GUI_SCENE_HPP_
