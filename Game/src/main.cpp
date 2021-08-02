#include <iostream>
#include <chrono>
#include <string>
#include <codecvt>

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
    Window window(1280, 720, "Window");

    auto context = window.createRenderContext(Swapchain::SwapchainMode::TRIPLE_BUFFER);
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
        context->getFontRenderer().draw(*font, getFpsString(), -0.99f, 0.99f);
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
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
        fpsString = conv.from_bytes("FPS: " + std::to_string(fpsCounter));
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