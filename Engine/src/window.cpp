#include "engine/window.hpp"

#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "engine/gui/input.hpp"
#include "engine/log.hpp"

#ifdef MCE_VK
#include "engine/vulkan/vulkan_context.hpp"
#endif

namespace engine {

Window::Window(int32_t width, int32_t height, const std::string_view& title) {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  auto* window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
  m_Window = WindowHandle(window, [](GLFWwindow* handle) { glfwDestroyWindow(handle); });

  auto file = NamespaceFile("minecraft", "textures/blocks/dirt.png");
  int imageWidth, imageHeight, channels;
  auto* buffer = stbi_load(file.getPath().c_str(), &imageWidth, &imageHeight, &channels, STBI_rgb_alpha);
  GLFWimage image{
      imageWidth,
      imageHeight,
      buffer
  };

  glfwSetWindowIcon(m_Window.get(), 1, &image);
  stbi_image_free(buffer);
}

std::unique_ptr<render::RenderContext> Window::createRenderContext(render::Swapchain::SwapchainMode modeHint) {
  //todo : PUT ENUM FOR RENDER APIS
#ifdef MCE_VK
  auto ptr = std::make_unique<vulkan::render::VulkanRenderContext>(*this, Directory("assets/minecraft"), modeHint);
#else
  std::unique_ptr<render::RenderContext> ptr = nullptr;
  throw std::runtime_error("no render api");
#endif

  auto currentMode = ptr->getSwapchain().getSwapchainMode();
  const auto& supportedModes = ptr->getSwapchain().getSupportedSwapchainModes();

  for (auto mode : supportedModes) {
    MCE_LOG_INFO("Supported Display Mode: {}", engine::render::Swapchain::toString(mode));
  }
  if (supportedModes.find(modeHint) == supportedModes.end()) {
    MCE_LOG_ERROR("Requested display mode [{}] not found, default to [{}]", engine::render::Swapchain::toString(modeHint), engine::render::Swapchain::toString(currentMode));
  } else {
    MCE_LOG_INFO("Selected display mode: [{}]", engine::render::Swapchain::toString(currentMode));
  }

  glfwSetWindowUserPointer(m_Window.get(), ptr.get());
  glfwSetWindowSizeCallback(m_Window.get(), [](GLFWwindow* handle, int width, int height) {
    auto* ref = reinterpret_cast<vulkan::render::RenderContext*>(glfwGetWindowUserPointer(handle));
    ref->windowResizeCallback(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
  });

  glfwSetMouseButtonCallback(m_Window.get(), [](GLFWwindow* handle, int button, int action, int modifier) {
    auto* ref = reinterpret_cast<vulkan::render::RenderContext*>(glfwGetWindowUserPointer(handle));

    auto buttonFlag = static_cast<engine::gui::input::MouseButton>(button);
    auto actionFlag = static_cast<engine::gui::input::MouseButtonAction>(action);
    auto modifierFlag = static_cast<engine::gui::input::Modifier>(modifier);

    double x, y;
    glfwGetCursorPos(handle, &x, &y);

    ref->mouseButtonCallback(buttonFlag, actionFlag, x, y);
  });

  glfwSetCursorPosCallback(m_Window.get(), [](GLFWwindow* handle, double x, double y){
    auto* ref = reinterpret_cast<vulkan::render::RenderContext*>(glfwGetWindowUserPointer(handle));
    ref->mousePositionCallback(x, y);
  });

  return std::move(ptr);
}

void Window::setCloseFlag(bool flag) {
  glfwSetWindowShouldClose(m_Window.get(), flag);
}

void Window::setCursorDisabledFlag(bool flag) {
  int mask = flag ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
  glfwSetInputMode(m_Window.get(), GLFW_CURSOR, mask);
}

bool Window::shouldClose() const {
  return glfwWindowShouldClose(m_Window.get());
}

std::pair<size_t, size_t> Window::getCursorPosition() const {
  double x, y;
  glfwGetCursorPos(m_Window.get(), &x, &y);

  return {static_cast<size_t>(std::floor(x)), static_cast<size_t>(std::floor(y))};
}

void Window::pollEvents() {
  glfwPollEvents();
}

#ifdef MCE_VK
vk::UniqueSurfaceKHR Window::createSurface(const vk::Instance& instance) const {
  VkSurfaceKHR surface;
  auto result = glfwCreateWindowSurface(instance, m_Window.get(), nullptr, &surface);
  vk::UniqueSurfaceKHR uniqueSurface((vk::SurfaceKHR(surface)), vk::ObjectDestroy<vk::Instance, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE>(instance));
  return std::move(uniqueSurface);
}

std::pair<const char**, uint32_t> Window::getExtensions() {
  uint32_t count;
  const char** extensions = glfwGetRequiredInstanceExtensions(&count);
  return {extensions, count};
}
#endif

}   // namespace engine