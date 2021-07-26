#include <iostream>
#include <chrono>

#include "engine/engine.hpp"
#include "engine/file.hpp"
#include "engine/window.hpp"

using engine::Window;
using engine::render::Swapchain;

struct Vertex {
    float x, y, z;
    float tx, ty;
};

template<typename T>
constexpr size_t size_of(size_t val) {
    return sizeof(T) * val;
}

void app_main() {
    engine::init();

    Window window(1920, 1080, "Window");

    auto context = window.createRenderContext(Swapchain::SwapchainMode::DOUBLE_BUFFER_VSYNC);
    auto pipeline = context->createRenderPipeline(engine::File("assets/shaders/basic"));
    auto pool = context->createCommandPool();

    std::array<Vertex, 4> vertices{
        Vertex{-0.5f, 0.5f, 0.1f, 0.0f, 1.0f},
        Vertex{-0.5f, -0.5f, 0.1f, 0.0f, 0.0f},
        Vertex{0.5f, -0.5f, 0.1f, 1.0f, 0.0f},
        Vertex{0.5f, 0.5f, 0.1f, 1.0f, 1.0f}
    };
    std::array<uint32_t, 6> indices{0, 1, 2, 0, 3, 2};

    auto vertexBuffer = context->allocateVertexBuffer(size_of<Vertex>(vertices.size()));
    vertexBuffer->write(0, vertices.data(), size_of<Vertex>(vertices.size()));

    auto indexBuffer = context->allocateIndexBuffer(indices.size());
    indexBuffer->write(0, indices.data(), indices.size());

    auto texture = context->createImage(engine::File(R"(C:\Users\Asyc\Downloads\hippo.jpg)"));

    auto uniformDescriptorSet = pipeline->allocateDescriptorSet(0);
    uniformDescriptorSet->bind(0, *texture);

    auto render = pool->allocateCommandListImmutable();
    render->begin();
    render->bindPipeline(*pipeline);
    render->bindUniformDescriptor(*uniformDescriptorSet);
    render->bindVertexBuffer(*vertexBuffer);
    render->bindIndexBuffer(*indexBuffer);
    render->drawIndexed(1, indices.size());
    render->end();

    auto fontBuffer = pool->allocateDirectCommandBuffer();

    auto buffer = pool->allocateIndirectCommandBuffer();

    size_t fps = 0;
    std::u16string stringFps = u"FPS: 0000";

    auto last = std::chrono::high_resolution_clock::now();

    while (!window.shouldClose()) {
        Window::pollEvents();

        fontBuffer->begin();
        context->getFontRenderer().draw(*fontBuffer, stringFps);
        fontBuffer->end();

        buffer->begin();
        buffer->accept(*render);
        buffer->end();

        context->getSwapchain().submitCommandBuffer(*buffer);
        context->getSwapchain().submitCommandBuffer(*fontBuffer);

        context->getSwapchain().nextImage();

        auto now = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last).count() >= 1000) {
            last = now;

            std::string numeric = std::to_string(fps);

            size_t index = 0;
            for (char i : numeric) {
                stringFps[index + 5] = (char16_t) i;
                index++;
            }

            stringFps[index + 5] = '\0';

            fps = 0;
        } else {
            fps++;
        }
    }

    engine::terminate();
}

int main() {
#ifdef MCE_DBG
    app_main();
#else
    try {
        app_main();
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return -1;
    }
#endif
}