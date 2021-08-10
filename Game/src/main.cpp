#include "engine/engine.hpp"
#include "engine/file.hpp"
#include "engine/window.hpp"

#include "engine/gui/scene.hpp"

#include "gui/main_menu.hpp"

#include "world/chunk.hpp"

using engine::Window;
using engine::render::Swapchain;

engine::render::RenderContext* g_RenderContext;

void app_main() {
  Window window(1280, 720, "Window");

  auto context = window.createRenderContext(Swapchain::SwapchainMode::TRIPLE_BUFFER);

  mc::BlockRegistry::initializeBlockRegistry(*context);
  g_RenderContext = context.get();

  auto pool = context->createCommandPool();
  auto font = pool->allocateDirectCommandBuffer();

  engine::gui::Scene scene(*context);
  scene.setGui<MainMenu>();

  while (!window.shouldClose()) {
    Window::pollEvents();

    scene.update();
    scene.render();

    // Submission
    //context->getSwapchain().submitCommandBuffer(*font);
    context->getSwapchain().nextImage();
  }
}
int main() {
#ifdef MCE_DBG
  engine::init();
  app_main();
  engine::terminate();
#else
  try {
      engine::init();
      app_main();
      engine::terminate();
      return 0;
  } catch (const std::exception& ex) {
      std::cerr << ex.what() << std::endl;
      return -1;
  }
#endif
}