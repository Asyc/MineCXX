#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_COMMAND_COMMAND_POOL_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_COMMAND_COMMAND_POOL_HPP_

#include <memory>

#include "command_buffer.hpp"

namespace engine::render::command {

class CommandPool {
public:
    virtual ~CommandPool() = default;

    virtual std::unique_ptr<DirectCommandBuffer> allocateDirectCommandBuffer() = 0;
    virtual std::unique_ptr<IndirectCommandBuffer> allocateIndirectCommandBuffer() = 0;

    virtual std::unique_ptr<CommandList> allocateCommandList() = 0;
    virtual std::unique_ptr<ImmutableCommandList> allocateCommandListImmutable() = 0;
};

}   // namespace engine::render::command

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_COMMAND_COMMAND_POOL_HPP_
