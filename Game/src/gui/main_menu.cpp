#include "main_menu.hpp"

using namespace engine;

MainMenu::MainMenu(engine::render::RenderContext& context)  {
    auto image = context.createImage(NamespaceFile("minecraft", "textures/gui/title/minecraft.png"));
    pushElement(std::make_unique<gui::ElementImage>(context, std::move(image), 0.0f, 0.0f));
}
