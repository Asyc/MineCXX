#ifndef MINECRAFTCXX_CLIENT_GAME_SRC_WORLD_BLOCK_HPP_
#define MINECRAFTCXX_CLIENT_GAME_SRC_WORLD_BLOCK_HPP_

#include <cstdint>
#include <unordered_map>

#include "engine/file.hpp"
#include "engine/render/render_context.hpp"

namespace mc {

class Block;

enum class BlockModel {
  STANDARD
};

class BlockRegistry {
 public:
  BlockRegistry(engine::render::RenderContext& context);

  Block* getBlock(size_t id);

  [[nodiscard]] const std::shared_ptr<engine::render::buffer::UniformBuffer>& getBlockRegistryBuffer() const { return m_BlockDataBuffer; }
  [[nodiscard]] const std::shared_ptr<engine::render::buffer::Image>& getAtlas() const { return m_Atlas; }

  static void initializeBlockRegistry(engine::render::RenderContext& context);
  static BlockRegistry& getInstance();

  std::shared_ptr<engine::render::buffer::UniformBuffer> m_BlockDataBuffer;
  std::shared_ptr<engine::render::buffer::Image> m_Atlas;
  std::unordered_map<size_t, std::unique_ptr<Block>> m_Blocks;
};

class BlockRenderer {
 public:

};
class StandardBlockRenderer : public BlockRenderer {

};

class Block {
 public:
  friend class BlockRegistry;

  Block(size_t id, engine::NamespaceFile texture);

  size_t getId() const { return m_Id; }

  virtual bool isOpaque() { return false; }

  BlockRegistry& getBlockRegistry() { return *m_Owner; }
  const BlockRegistry& getBlockRegistry() const { return *m_Owner; }

  const engine::File& getTexture() { return m_Texture; }
 protected:
  BlockRegistry* m_Owner;

  size_t m_Id;
  engine::NamespaceFile m_Texture;

  glm::vec4 m_AtlasPosition;
};

class BlockDirt : public Block {
 public:
  BlockDirt() : Block(3, engine::NamespaceFile("minecraft", "textures/blocks/dirt.png")) {}
};

}

#endif //MINECRAFTCXX_CLIENT_GAME_SRC_WORLD_BLOCK_HPP_
