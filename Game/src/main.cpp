#include "engine/engine.hpp"
#include "engine/file.hpp"
#include "engine/window.hpp"

#include <chrono>

using engine::Window;
using engine::render::Swapchain;

struct Vertex {
    float x, y, z;
};

int main() {
    engine::init();

    Window window(1920, 1080, "Window");

    auto context = window.createRenderContext(Swapchain::SwapchainMode::DOUBLE_BUFFER);
    auto pipeline = context->createRenderPipeline(engine::File("assets/shaders/basic"));
    auto pool = context->createCommandPool();

    std::array<Vertex, 3> vertices{
        Vertex{-0.5f, -0.5f, 0.0f},
        Vertex{ 0.0f,  0.5f, 0.0f},
        Vertex{ 0.5f, -0.5f, 0.0f}
    };

    auto vertexBuffer = context->allocateVertexBuffer(vertices.size() * sizeof(Vertex));
    vertexBuffer->write(0, vertices.data(), vertices.size() * sizeof(Vertex));

    auto render = pool->allocateCommandListImmutable();
    render->begin();
    render->bindPipeline(*pipeline);
    render->bindVertexBuffer(*vertexBuffer);
    render->draw(1, 3);
    render->end();

    auto buffer = pool->allocateIndirectCommandBuffer();

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