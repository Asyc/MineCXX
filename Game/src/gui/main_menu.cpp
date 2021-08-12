#include "main_menu.hpp"

#include <array>

#include "engine/window.hpp"

#include "world/block.hpp"
#include "world/chunk.hpp"

using namespace engine;
using namespace engine::gui;

MainMenu::MainMenu(engine::render::RenderContext& context, engine::gui::Scene& scene) {
  auto file = NamespaceFile("minecraft", "textures/gui/title/minecraft.png");
  auto image = context.createImage(file);

  auto backgroundImage = context.createImage(NamespaceFile("minecraft", "textures/gui/options_background.png"));

  auto[windowWidth, windowHeight] = context.getSwapchain().getSize();
  auto[backgroundWidth, backgroundHeight] = backgroundImage->getSize();
  float horizontalCount = static_cast<float>(windowWidth) / static_cast<float>(backgroundWidth) * 4;
  float verticalCount = static_cast<float>(windowHeight) / static_cast<float>(backgroundHeight) * 4;

  float x = 1.0f / 3.9f;
  //float x = 15.0f / 255.0f;
  gui::ElementImage::Options options = {x, x, x, x, false};

  pushElement(std::make_unique<gui::ElementImage>(context, std::move(backgroundImage), -1.f, 1.0f, 2.0f, 2.0f, ElementImage::ImageRegion{0.0f, 0.0f, horizontalCount, verticalCount}, options));

  float width = 0.6f, height = 0.2f;
  pushElement(std::make_unique<gui::ElementImage>(context, image, -width, 0.7f, width, height, ElementImage::ImageRegion{0.0f, 0.0f, 155.0f, 45.0f}));
  pushElement(std::make_unique<gui::ElementImage>(context, image, 0.0f, 0.7f, width, height, ElementImage::ImageRegion{0.0f, 45.0f, 119.0f, 45.0f}));

  float ratio = 20.0f / 200.0f;
  float w = 0.8f, h = ratio * w;

  float y = 0.3f;
  float buttonOffset = h / 5.f;

  auto button = std::make_unique<gui::ElementButton>(context, 0.0f - w / 2, y, w, h);
  float leftBound = button->getX();
  button->setText(u"Singleplayer");

  button->setButtonCallback([&]() {
    std::shared_ptr<mc::Chunk> chunk = std::make_shared<mc::Chunk>(0, 0);
    auto block = mc::BlockRegistry::getInstance().getBlock(3);

    chunk->setBlock(5, 0, 0, block);


    scene.setGui(nullptr);
    scene.setWorldObject(chunk);
  });

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
  button->setButtonCallback([&context]() {
    context.getWindow().setCloseFlag(true);
  });

  pushElement(std::move(button));
}

MainMenu::~MainMenu() {
}
