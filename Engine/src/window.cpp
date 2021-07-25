#include "engine/window.hpp"

#include <GLFW/glfw3.h>

#include "engine/gui/input.hpp"
#include "engine/log.hpp"
#include "engine/vulkan/vulkan_context.hpp"

namespace engine {

Window::Window(int32_t width, int32_t height, const std::string_view& title) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    auto* window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
    m_Window = WindowHandle(window, [](GLFWwindow* handle) { glfwDestroyWindow(handle); });
}

std::unique_ptr<render::RenderContext> Window::createRenderContext(render::Swapchain::SwapchainMode modeHint) {
    //todo : PUT ENUM FOR RENDER APIS
    auto ptr = std::make_unique<vulkan::render::VulkanRenderContext>(*this, Directory("assets/minecraft"), modeHint);

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
    glfwSetWindowSizeCallback(m_Window.get(), [](GLFWwindow* handle, int width, int height){
        auto* ref = reinterpret_cast<vulkan::render::VulkanRenderContext*>(glfwGetWindowUserPointer(handle));
        ref->getSwapchain().onResize(width, height);
    });

    glfwSetMouseButtonCallback(m_Window.get(), [](GLFWwindow* handle, int button, int action, int modifier){
        auto* ref = reinterpret_cast<vulkan::render::VulkanRenderContext*>(glfwGetWindowUserPointer(handle));

        engine::gui::input::MouseButton buttonFlag;
        switch (button) {
            case GLFW_MOUSE_BUTTON_LEFT:
                buttonFlag = gui::input::MouseButton::LEFT;
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                buttonFlag = gui::input::MouseButton::RIGHT;
                break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                buttonFlag = gui::input::MouseButton::MIDDLE;
                break;
            default: return;
        }

        engine::gui::input::MouseButtonAction actionFlag;
        switch (action) {
            case GLFW_PRESS:
                actionFlag = gui::input::MouseButtonAction::PRESS;
                break;
            case GLFW_RELEASE:
                actionFlag = gui::input::MouseButtonAction::RELEASE;
                break;
            default:
                return;
        }

        double x, y;
        glfwGetCursorPos(handle, &x, &y);

        ref->mouseButtonCallback(buttonFlag, actionFlag, x, y);
    });

    return std::move(ptr);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_Window.get());
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