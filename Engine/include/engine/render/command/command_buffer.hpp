#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_COMMAND_COMMAND_BUFFER_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_COMMAND_COMMAND_BUFFER_HPP_

#include "engine/render/buffer/vertex_buffer.hpp"
#include "engine/render/pipeline.hpp"

namespace engine::render::command {

class ICommandBuffer {
public:
    virtual ~ICommandBuffer() = default;

    virtual void begin() = 0;
    virtual void end() = 0;
};

class IDrawableCommandBuffer : public virtual ICommandBuffer {
public:
    virtual void bindPipeline(const RenderPipeline& pipeline) = 0;
    virtual void bindVertexBuffer(const buffer::VertexBuffer& buffer) = 0;
    virtual void draw(uint32_t instanceCount, uint32_t vertexCount) = 0;
};

class ISubmittableCommandBuffer : public virtual ICommandBuffer {};

class CommandList : public virtual IDrawableCommandBuffer {};
class ImmutableCommandList : public virtual IDrawableCommandBuffer {};

class DirectCommandBuffer : public virtual IDrawableCommandBuffer, public virtual ISubmittableCommandBuffer {

};

class IndirectCommandBuffer : public virtual ISubmittableCommandBuffer {
public:
    virtual void accept(const CommandList& commandList) = 0;
    virtual void accept(const ImmutableCommandList& commandList) = 0;
};




}   // namespace engine::render::command

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_COMMAND_COMMAND_BUFFER_HPP_
