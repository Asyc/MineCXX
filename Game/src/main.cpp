#include "engine/engine.hpp"
#include "engine/file.hpp"
#include "engine/window.hpp"
#include "engine/log.hpp"
#include "engine/render/render_context.hpp"

using engine::Window;
using engine::render::Swapchain;

int main() {
    engine::init();

    Window window(1920, 1080, "Window");

    auto context = window.createRenderContext(Swapchain::SwapchainMode::DOUBLE_BUFFER);
    auto pipeline = context->createRenderPipeline(engine::File(R"(C:/Users/Asyc/Desktop/SPIRV)"));
    auto pool = context->createCommandPool();
    auto buffer = pool->allocateDirectCommandBuffer();

    while (!window.shouldClose()) {
        Window::pollEvents();

        buffer->begin();
        //buffer->bindPipeline(*pipeline);
        buffer->end();

        context->getSwapchain().submitCommandBuffer(*buffer);
        context->getSwapchain().nextImage();
    }

    engine::terminate();
}