#include "main_menu.hpp"

#include <array>

#include <stb_image.h>

using namespace engine;
using namespace engine::gui;

MainMenu::MainMenu(engine::render::RenderContext& context) {
    auto file = NamespaceFile("minecraft", "textures/gui/title/minecraft.png");
    auto image = context.createImage(file);

    auto backgroundImage = context.createImage(NamespaceFile("minecraft", "textures/gui/options_background.png"));

    auto [windowWidth, windowHeight] = context.getSwapchain().getSize();
    auto [backgroundWidth, backgroundHeight] = backgroundImage->getSize();
    float horizontalCount = static_cast<float>(windowWidth) / static_cast<float>(backgroundWidth) * 3;
    float verticalCount = static_cast<float>(windowHeight) / static_cast<float>(backgroundHeight) * 3;

    pushElement(std::make_unique<gui::ElementImage>(context, std::move(backgroundImage), -1.f, 1.0f, 2.0f, 2.0f, ElementImage::ImageRegion{0.0f, 0.0f, horizontalCount, verticalCount}));

    float width = 0.601f, height = 0.2f;
    pushElement(std::make_unique<gui::ElementImage>(context, image, -0.6f, 0.6f, width, height, ElementImage::ImageRegion{0.0f, 0.0f, 155.0f, 45.0f}));
    pushElement(std::make_unique<gui::ElementImage>(context, image, 0.0f, 0.6f, width, height, ElementImage::ImageRegion{0.0f, 45.0f, 119.0f, 45.0f}));
}
