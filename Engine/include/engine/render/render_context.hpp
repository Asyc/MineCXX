#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_RENDER_CONTEXT_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_RENDER_CONTEXT_HPP_

#include <memory>

#include "buffer/image.hpp"
#include "buffer/index_buffer.hpp"
#include "buffer/uniform_buffer.hpp"
#include "buffer/storage_buffer.hpp"
#include "buffer/vertex_buffer.hpp"
#include "command/command_pool.hpp"
#include "engine/file.hpp"
#include "engine/gui/font/font_renderer.hpp"
#include "engine/gui/input.hpp"

#include "camera.hpp"
#include "pipeline.hpp"
#include "swapchain.hpp"
#include "viewport.hpp"

namespace engine {
class Window;
}

namespace engine::render {

class ViewportGUI;

class RenderContext {
 public:
  using ResizeCallback = std::function<void(uint32_t, uint32_t)>;
  using MouseCallback = engine::gui::input::MouseButtonCallback;
  using MousePositionCallback = engine::gui::input::MousePositionCallback;

  virtual ~RenderContext() = default;

  [[nodiscard]] virtual std::shared_ptr<buffer::Image> createImage(const File& path, const buffer::Image::SamplerOptions& samplerOptions) = 0;
  [[nodiscard]] virtual std::shared_ptr<buffer::Image> createImage(const File& path) {
    return createImage(path, buffer::Image::SamplerOptions());
  }

  [[nodiscard]] virtual std::shared_ptr<buffer::Image> createImage(void* buffer, uint32_t width, uint32_t height, const buffer::Image::SamplerOptions& samplerOptions) = 0;

  [[nodiscard]] virtual std::unique_ptr<buffer::VertexBuffer> allocateVertexBuffer(size_t size) = 0;

  [[nodiscard]] virtual std::unique_ptr<buffer::IndexBuffer> allocateIndexBuffer(size_t size) = 0;
  [[nodiscard]] virtual std::unique_ptr<buffer::UniformBuffer> allocateUniformBuffer(size_t size) = 0;
  [[nodiscard]] virtual std::unique_ptr<buffer::StorageBuffer> allocateStorageBuffer(size_t size) = 0;

  [[nodiscard]] virtual std::shared_ptr<RenderPipeline> createRenderPipeline(const File& file) = 0;

  [[nodiscard]] virtual std::unique_ptr<command::CommandPool> createCommandPool() const = 0;
  [[nodiscard]] virtual command::CommandPool& getThreadCommandPool() = 0;

  [[nodiscard]] virtual Swapchain& getSwapchain() = 0;
  [[nodiscard]] virtual const Swapchain& getSwapchain() const = 0;

  [[nodiscard]] virtual const ViewportGUI& getViewport() const = 0;

  [[nodiscard]] virtual Camera& getCamera() = 0;
  [[nodiscard]] virtual const Camera& getCamera() const = 0;

  [[nodiscard]] virtual gui::font::FontRenderer& getFontRenderer() = 0;

  [[nodiscard]] virtual Window& getWindow() = 0;
  [[nodiscard]] virtual const Window& getWindow() const = 0;

  virtual void addResizeCallback(ResizeCallback callback) = 0;
  virtual void addMouseCallback(MouseCallback callback) = 0;
  virtual void addMousePositionCallback(MousePositionCallback callback) = 0;

  // Callback functions
  virtual void mouseButtonCallback(gui::input::MouseButton button, gui::input::MouseButtonAction action, double x, double y) = 0;
  virtual void mousePositionCallback(double x, double y)  = 0;
  virtual void windowResizeCallback(uint32_t width, uint32_t height) = 0;
 protected:
  RenderContext() = default;
};

}   // namespace engine::render

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_RENDER_CONTEXT_HPP_
