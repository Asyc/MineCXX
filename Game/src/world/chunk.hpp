#ifndef MINECRAFTCXX_CLIENT_GAME_SRC_WORLD_CHUNK_HPP_
#define MINECRAFTCXX_CLIENT_GAME_SRC_WORLD_CHUNK_HPP_

#include <array>
#include <memory>

#include "block.hpp"
#include "engine/world_object.hpp"
#include "engine/render/command/command_buffer.hpp"

namespace mc {

class Chunk : public engine::WorldObject {
 public:
  struct BlockContainer {
    Block* blockType;
  };

  Chunk(int32_t x, int32_t y);

  void update() override;
  void tick() override;
  void draw(engine::IDrawableCommandBuffer& commandBuffer) override;
 public:
  void setBlock(int32_t x, int32_t y, int32_t z, Block* blockType);
 private:
  int32_t m_X, m_Y;
  std::array<std::array<std::array<BlockContainer, 16>, 256>, 16> m_BlockData;
  size_t m_BlockCount;

  std::shared_ptr<engine::render::RenderPipeline> m_Pipeline;
  std::unique_ptr<engine::render::UniformDescriptor> m_UniformDescriptor;
  std::unique_ptr<engine::render::buffer::VertexBuffer> m_BlockVbo;
  std::unique_ptr<engine::render::buffer::IndexBuffer> m_BlockIbo;

  std::unique_ptr<engine::render::buffer::StorageBuffer> m_StorageBuffer;

  std::unique_ptr<engine::render::command::CommandList> m_RenderChunk;
};

}

#endif //MINECRAFTCXX_CLIENT_GAME_SRC_WORLD_CHUNK_HPP_
