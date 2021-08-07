#ifndef MINECRAFTCXX_CLIENT_GAME_SRC_GUI_MAIN_MENU_HPP_
#define MINECRAFTCXX_CLIENT_GAME_SRC_GUI_MAIN_MENU_HPP_

#include "engine/gui/gui.hpp"
#include "engine/gui/scene.hpp"
#include "engine/render/command/command_buffer.hpp"

class MainMenu : public engine::gui::Gui {
 public:
  explicit MainMenu(engine::render::RenderContext& context, engine::gui::Scene& scene);
};

#endif //MINECRAFTCXX_CLIENT_GAME_SRC_GUI_MAIN_MENU_HPP_
