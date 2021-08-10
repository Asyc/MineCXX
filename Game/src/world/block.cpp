#include "block.hpp"

#include <glm/vec4.hpp>

#include "engine/image/packer.hpp"

static std::shared_ptr<mc::BlockRegistry> g_BlockRegistry;

namespace mc {

struct BlockData {
  glm::vec4 texAtlasPosition;
};

struct Container {
  engine::image::TexturePacker::Texture* texture;
  Block* block;
};

BlockRegistry::BlockRegistry(engine::render::RenderContext& context) {
  engine::image::TexturePacker packer(context);
  std::vector<Container> textureHandles;

  auto registerBlock = [&](std::unique_ptr<Block> block) {
    textureHandles.push_back({packer.addImage(block->getTexture()), block.get()});
    m_Blocks[block->getId()] = std::move(block);
  };

  registerBlock(std::make_unique<BlockDirt>());

  m_Atlas = packer.stitch();
  auto atlasWidth = static_cast<float>(m_Atlas->getWidth());
  auto atlasHeight = static_cast<float>(m_Atlas->getHeight());

  m_BlockDataBuffer = context.allocateUniformBuffer(sizeof(BlockData) * m_Blocks.size());

  auto buffer = std::make_unique<BlockData[]>(m_Blocks.size());
  for (size_t i = 0; i < m_Blocks.size(); i++) {
    auto& container = textureHandles[i];
    auto* texture = container.texture;

    auto x = static_cast<float>(texture->x) / atlasWidth;
    auto y = static_cast<float>(texture->y) / atlasHeight;
    auto w = static_cast<float>(texture->width) / atlasWidth;
    auto h = static_cast<float>(texture->height) / atlasHeight;

    buffer[i] = BlockData{glm::vec4(x, y, w, h)};
    container.block->m_AtlasPosition = glm::vec4(x, y, w, h);
  }

  m_BlockDataBuffer->write(0, buffer.get(), sizeof(BlockData) * m_Blocks.size());
}

Block* BlockRegistry::getBlock(size_t id) {
  return m_Blocks.find(id)->second.get();
}

void BlockRegistry::initializeBlockRegistry(engine::render::RenderContext& context) {
  g_BlockRegistry = std::make_shared<BlockRegistry>(context);
}

BlockRegistry& BlockRegistry::getInstance() {
  return *g_BlockRegistry;
}

Block::Block(size_t id, engine::NamespaceFile texture) : m_Id(id), m_Texture(std::move(texture)) {

}


}