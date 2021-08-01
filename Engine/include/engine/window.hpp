#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_WINDOW_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_WINDOW_HPP_

#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>
#include <tuple>

#include "render/render_context.hpp"
#include "render/swapchain.hpp"

#ifdef MCE_VK
#include <vulkan/vulkan.hpp>
#endif

struct GLFWwindow;

namespace engine {

using WindowHandle = std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>>;

class Window {
public:
    Window(int32_t width, int32_t height, const std::string_view& title);

    std::unique_ptr<render::RenderContext> createRenderContext(render::Swapchain::SwapchainMode modeHint);

    void setCloseFlag(bool flag);
    [[nodiscard]] bool shouldClose() const;

    [[nodiscard]] std::pair<size_t, size_t> getCursorPosition() const;

#ifdef MCE_VK
    [[nodiscard]] vk::UniqueSurfaceKHR createSurface(const vk::Instance& instance) const;

    static std::pair<const char**, uint32_t> getExtensions();
#endif

    static void pollEvents();
private:
    WindowHandle m_Window;

};

}   // namespace window

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_WINDOW_HPP_
