#include "engine/engine.hpp"
#include "engine/file.hpp"
#include "engine/window.hpp"

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

    std::array<Vertex, 4> vertices{
        Vertex{-0.5f, 0.5f, 0.0f},
        Vertex{-0.5f, -0.5f, 0.0f},
        Vertex{ 0.5f, -0.5f, 0.0f},
        Vertex{0.5f, 0.5f, 0.0f}
    };
    std::array<uint32_t, 6> indices{0, 1, 2, 0, 3, 2};

    auto vertexBuffer = context->allocateVertexBuffer(vertices.size() * sizeof(Vertex));
    vertexBuffer->write(0, vertices.data(), vertices.size() * sizeof(Vertex));

    auto indexBuffer = context->allocateIndexBuffer(indices.size());
    indexBuffer->write(0, indices.data(), indices.size());

    auto render = pool->allocateCommandListImmutable();
    render->begin();
    render->bindPipeline(*pipeline);
    render->bindVertexBuffer(*vertexBuffer);
    render->bindIndexBuffer(*indexBuffer);
    render->drawIndexed(1, indices.size());
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