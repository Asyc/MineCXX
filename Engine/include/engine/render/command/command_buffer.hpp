#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_COMMAND_COMMAND_BUFFER_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_COMMAND_COMMAND_BUFFER_HPP_

#include "engine/render/buffer/index_buffer.hpp"
#include "engine/render/buffer/uniform_buffer.hpp"
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
  enum class PushConstantUsage : uint32_t {
    VERTEX = 2, GEOMETRY = 4, FRAGMENT = 8
  };

  virtual void bindPipeline(const RenderPipeline& pipeline) = 0;

  virtual void bindVertexBuffer(const buffer::VertexBuffer& buffer) = 0;
  virtual void bindIndexBuffer(const buffer::IndexBuffer& buffer) = 0;
  virtual void bindUniformDescriptor(const UniformDescriptor& descriptor) = 0;

  virtual void draw(uint32_t instanceCount, uint32_t vertexCount) = 0;
  virtual void drawIndexed(uint32_t instanceCount, uint32_t indexCount) = 0;

  virtual void pushConstants(PushConstantUsage usage, uint32_t offset, const void* data, size_t length) = 0;
};

using PushConstantUsage = IDrawableCommandBuffer::PushConstantUsage;

inline IDrawableCommandBuffer::PushConstantUsage operator|(IDrawableCommandBuffer::PushConstantUsage lhs, IDrawableCommandBuffer::PushConstantUsage rhs) {
  return static_cast<IDrawableCommandBuffer::PushConstantUsage> (
      static_cast<std::underlying_type<IDrawableCommandBuffer::PushConstantUsage>::type>(lhs) | static_cast<std::underlying_type<IDrawableCommandBuffer::PushConstantUsage>::type>(rhs)
  );
}

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
