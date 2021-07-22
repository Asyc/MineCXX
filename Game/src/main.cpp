#include <iostream>

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

#include <bitset>

void app_main() {
    engine::init();

    Window window(1920, 1080, "Window");

    auto context = window.createRenderContext(Swapchain::SwapchainMode::DOUBLE_BUFFER);
    auto pipeline = context->createRenderPipeline(engine::File("assets/shaders/basic"));
    auto pool = context->createCommandPool();

    std::array<Vertex, 4> vertices{
        Vertex{-0.5f, 0.5f, 0.0f, 0.0f, 1.0f},
        Vertex{-0.5f, -0.5f, 0.0f, 0.0f, 0.0f},
        Vertex{0.5f, -0.5f, 0.0f, 1.0f, 0.0f},
        Vertex{0.5f, 0.5f, 0.0f, 1.0f, 1.0f}
    };
    std::array<uint32_t, 6> indices{0, 1, 2, 0, 3, 2};

    std::array<float, 4> color{1.0f, 1.0f, 1.0f, 1.0f};
    std::array<uint32_t, 1> sampler{0};

    auto vertexBuffer = context->allocateVertexBuffer(size_of<Vertex>(vertices.size()));
    vertexBuffer->write(0, vertices.data(), size_of<Vertex>(vertices.size()));

    auto indexBuffer = context->allocateIndexBuffer(indices.size());
    indexBuffer->write(0, indices.data(), indices.size());

    auto uniformBuffer = context->allocateUniformBuffer(*pipeline, size_of<float>(color.size()));
    uniformBuffer->write(0, color.data(), size_of<float>(color.size()));

    auto render = pool->allocateCommandListImmutable();
    render->begin();
    render->bindPipeline(*pipeline);
    render->bindUniformBuffer(*uniformBuffer, 0);
    render->bindVertexBuffer(*vertexBuffer);
    render->bindIndexBuffer(*indexBuffer);
    render->drawIndexed(1, indices.size());
    render->end();

    auto buffer = pool->allocateIndirectCommandBuffer();
    auto texture = context->createImage(engine::File(R"(C:\Users\Asyc\Documents\backup\pic.jpg)"));

    while (!window.shouldClose()) {
        Window::pollEvents();

        buffer->begin();
        buffer->accept(*render);
        buffer->end();

        context->getSwapchain().submitCommandBuffer(*buffer);
        context->getSwapchain().nextImage();
    }

    engine::terminate();
}

int main() {
    try {
        app_main();
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return -1;
    }
}