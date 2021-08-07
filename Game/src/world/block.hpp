#ifndef MINECRAFTCXX_CLIENT_GAME_SRC_WORLD_BLOCK_HPP_
#define MINECRAFTCXX_CLIENT_GAME_SRC_WORLD_BLOCK_HPP_

#include <cstdint>
#include <unordered_map>

#include "engine/file.hpp"

namespace mc {

class Block;

class BlockRegistry {
 public:
  BlockRegistry();
  Block* getBlock(size_t id);
 private:
  std::unordered_map<size_t, std::unique_ptr<Block>> m_Blocks;
};

class Block {
 public:
  friend class BlockRegistry;

  Block(size_t id, engine::NamespaceFile texture);

  size_t getId() const { return m_Id; }
 protected:
  size_t m_Id;
  engine::NamespaceFile m_Texture;

  struct {
    uint32_t x, y, w, h;
  } atlasPosition;
};

class BlockDirt : public Block {
 public:
  BlockDirt() : Block(3, engine::NamespaceFile("minecraft", "textures/blocks/dirt.png")) {}
};

}

#endif //MINECRAFTCXX_CLIENT_GAME_SRC_WORLD_BLOCK_HPP_
