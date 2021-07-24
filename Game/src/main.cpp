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

struct DummyStruct {
        uint8_t codepoint;

        struct {
            float x, y;
        };
};

void app_main() {
    engine::init();

    LOG_INFO("SIZE: {}", sizeof(DummyStruct));

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

    auto fontBuffer = pool->allocateCommandListImmutable();
    fontBuffer->begin();
    context->getFontRenderer().draw(*fontBuffer, u"FPS: 0000");
    fontBuffer->end();

    auto buffer = pool->allocateIndirectCommandBuffer();

    while (!window.shouldClose()) {
        Window::pollEvents();

        buffer->begin();
        //buffer->accept(*render);
        buffer->accept(*fontBuffer);
        buffer->end();

        context->getSwapchain().submitCommandBuffer(*buffer);
        context->getSwapchain().nextImage();
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