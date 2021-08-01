#include "main_menu.hpp"

#include <array>

#include "engine/window.hpp"

using namespace engine;
using namespace engine::gui;

MainMenu::MainMenu(engine::render::RenderContext& context) {
    auto file = NamespaceFile("minecraft", "textures/gui/title/minecraft.png");
    auto image = context.createImage(file);

    auto backgroundImage = context.createImage(NamespaceFile("minecraft", "textures/gui/options_background.png"));

    auto[windowWidth, windowHeight] = context.getSwapchain().getSize();
    auto[backgroundWidth, backgroundHeight] = backgroundImage->getSize();
    float horizontalCount = static_cast<float>(windowWidth) / static_cast<float>(backgroundWidth) * 3;
    float verticalCount = static_cast<float>(windowHeight) / static_cast<float>(backgroundHeight) * 3;

    float x = 15.0f / 255.0f;
    gui::ElementImage::Options options = {x, x, x, 1.0f};

    pushElement(std::make_unique<gui::ElementImage>(context, std::move(backgroundImage), -1.f, 1.0f, 2.0f, 2.0f, ElementImage::ImageRegion{0.0f, 0.0f, horizontalCount, verticalCount}, options));

    float width = 0.4f, height = 0.2f;
    pushElement(std::make_unique<gui::ElementImage>(context, image, -0.4f, 0.8f, width, height, ElementImage::ImageRegion{0.0f, 0.0f, 155.0f, 45.0f}));
    pushElement(std::make_unique<gui::ElementImage>(context, image, 0.0f, 0.8f, width, height, ElementImage::ImageRegion{0.0f, 45.0f, 119.0f, 45.0f}));

    float ratio = 20.0f / 200.0f;
    float w = 0.8f, h = ratio * w;
    w = 0.5f;

    float y = 0.3f;
    float buttonOffset = h / 5.f;

    auto button = std::make_unique<gui::ElementButton>(context, 0.0f - w / 2, y, w, h);
    float leftBound = button->getX();
    button->setText(u"Singleplayer");
    pushElement(std::move(button));

    y -= h + buttonOffset;

    button = std::make_unique<gui::ElementButton>(context, 0.0f - w / 2, y, w, h);
    button->setText(u"Multiplayer");
    pushElement(std::move(button));

    y -= h + buttonOffset;

    button = std::make_unique<gui::ElementButton>(context, 0.0f - w / 2, y, w, h);
    button->setText(u"Minecraft Realms");
    pushElement(std::move(button));

    buttonOffset = h * 0.59f;
    y -= h + buttonOffset;

    float blank = w * 0.05f;

    w -= blank;
    w /= 2.0f;

    button = std::make_unique<gui::ElementButton>(context, leftBound, y, w, h);
    button->setText(u"Options");
    pushElement(std::move(button));

    button = std::make_unique<gui::ElementButton>(context, leftBound + w + blank, y, w, h);
    button->setText(u"Quit Game");
    button->setButtonCallback([&context](){
        context.getWindow().setCloseFlag(true);
    });


    pushElement(std::move(button));
}
