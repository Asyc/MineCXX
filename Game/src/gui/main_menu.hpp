#ifndef MINECRAFTCXX_CLIENT_GAME_SRC_GUI_MAIN_MENU_HPP_
#define MINECRAFTCXX_CLIENT_GAME_SRC_GUI_MAIN_MENU_HPP_

#include "engine/gui/gui.hpp"

class MainMenu : public engine::gui::Gui {
public:
    class TitleImageElement : public engine::gui::Element {

    };

    explicit MainMenu(engine::render::RenderContext& context);
};

#endif //MINECRAFTCXX_CLIENT_GAME_SRC_GUI_MAIN_MENU_HPP_
