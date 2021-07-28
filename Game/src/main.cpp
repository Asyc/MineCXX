#include <iostream>
#include <chrono>

#include "engine/engine.hpp"
#include "engine/file.hpp"
#include "engine/window.hpp"

#include "engine/gui/scene.hpp"

#include "gui/main_menu.hpp"

using engine::Window;
using engine::render::Swapchain;

void tickFps();
const std::u16string& getFpsString();

void app_main() {
    Window window(1920, 1080, "Window");

    auto context = window.createRenderContext(Swapchain::SwapchainMode::DOUBLE_BUFFER);
    auto pipeline = context->createRenderPipeline(engine::File("assets/shaders/basic"));
    auto pool = context->createCommandPool();

    auto texture = context->createImage(engine::File(R"(C:\Users\Asyc\Downloads\hippo.jpg)"));
    auto uniformDescriptorSet = pipeline->allocateDescriptorSet(0);
    uniformDescriptorSet->bind(0, *texture);

    auto font = pool->allocateDirectCommandBuffer();

    engine::gui::Scene scene(*context);
    scene.setGui<MainMenu>();

    while (!window.shouldClose()) {
        Window::pollEvents();

        scene.update();
        scene.render();

        font->begin();
        context->getFontRenderer().draw(*font, getFpsString());
        font->end();

        // Submission
        context->getSwapchain().submitCommandBuffer(*font);
        context->getSwapchain().nextImage();

        tickFps();
    }
}

std::chrono::high_resolution_clock::time_point lastTick = std::chrono::high_resolution_clock::now();
std::u16string fpsString = u"FPS: 0000";
size_t fpsCounter = 0;

void tickFps() {
    auto now = std::chrono::high_resolution_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(now - lastTick).count() >= 1) {
        std::string numeric = std::to_string(fpsCounter);

        for (size_t i = 0; i < numeric.size(); i++) {
            fpsString[i + 5] = static_cast<char16_t>(numeric[i]);
        }

        fpsString[numeric.size() + 5] = u'\0';

        fpsCounter = 0;
        lastTick = now;
    } else {
        fpsCounter++;
    }
}

const std::u16string& getFpsString() {
    return fpsString;
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