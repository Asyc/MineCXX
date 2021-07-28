#include "main_menu.hpp"

#include <array>

using namespace engine;
using namespace engine::gui;

MainMenu::MainMenu(engine::render::RenderContext& context) {
    auto file = NamespaceFile("minecraft", "textures/gui/title/minecraft.png");
    auto image = context.createImage(file);

    float y = 0.6f;

    pushElement(std::make_unique<gui::ElementImage>(context, image, -0.6f, y, ElementImage::ImageRegion{0.0f, 0.0f, 155.0f, 45.0f}));
    pushElement(std::make_unique<gui::ElementImage>(context, image, 0.0f, y, ElementImage::ImageRegion{0.0f, 45.0f, 119.0f, 45.0f}));
}
