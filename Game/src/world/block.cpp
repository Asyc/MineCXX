#include "block.hpp"

namespace mc {

BlockRegistry::BlockRegistry() {
  auto registerBlock = [this](std::unique_ptr<Block> block) {
    m_Blocks[block->getId()] = std::move(block);
  };

  registerBlock(std::make_unique<BlockDirt>());
}

Block* BlockRegistry::getBlock(size_t id) {
  return &m_Blocks.find(id)->second;
}

Block::Block(size_t id, engine::NamespaceFile texture) : m_Id(id), m_Texture(std::move(texture)) {

}

}