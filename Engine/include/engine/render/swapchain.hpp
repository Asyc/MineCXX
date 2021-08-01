#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_SWAPCHAIN_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_SWAPCHAIN_HPP_

#include <string_view>
#include <set>
#include <tuple>

#include "command/command_buffer.hpp"

namespace engine::render {

class Swapchain {
public:
    enum class SwapchainMode : uint8_t {
        DOUBLE_BUFFER,
        TRIPLE_BUFFER,
        DOUBLE_BUFFER_VSYNC,
        TRIPLE_BUFFER_VSYNC
    };

    virtual ~Swapchain() = default;

    virtual void nextImage() = 0;

    virtual void submitCommandBuffer(const command::DirectCommandBuffer& buffer) = 0;
    virtual void submitCommandBuffer(const command::IndirectCommandBuffer& buffer) = 0;

    virtual void onResize(uint32_t width, uint32_t height) = 0;

    [[nodiscard]] virtual SwapchainMode getSwapchainMode() const = 0;
    [[nodiscard]] virtual const std::set<SwapchainMode>& getSupportedSwapchainModes() const = 0;

    [[nodiscard]] virtual uint32_t getCurrentFrameIndex() const = 0;
    [[nodiscard]] virtual uint32_t getFrameCount() const = 0;

    [[nodiscard]] virtual std::pair<uint32_t, uint32_t> getSize() const = 0;

    static std::string_view toString(SwapchainMode mode) {
        std::string_view string;
        switch (mode) {
            case render::Swapchain::SwapchainMode::DOUBLE_BUFFER:string = "Double Buffer";
                break;
            case render::Swapchain::SwapchainMode::TRIPLE_BUFFER:string = "Triple Buffer";
                break;
            case render::Swapchain::SwapchainMode::DOUBLE_BUFFER_VSYNC:string = "Double Buffer (VSYNC)";
                break;
            case render::Swapchain::SwapchainMode::TRIPLE_BUFFER_VSYNC:string = "Triple Buffer (VSYNC)";
                break;
        }

        return string;
    }
protected:
    Swapchain() = default;
};

}   // namespace engine::render

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_SWAPCHAIN_HPP_
