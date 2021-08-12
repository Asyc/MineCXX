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
    Block* handle = block.get();
    m_Blocks[block->getId()] = std::move(block);
    if (handle->getId() == 0) return;
    textureHandles.push_back({packer.addImage(handle->getTexture()), handle});
  };

  registerBlock(std::make_unique<BlockAir>());
  registerBlock(std::make_unique<BlockStone>());
  registerBlock(std::make_unique<BlockGrass>());
  registerBlock(std::make_unique<BlockDirt>());

  m_Atlas = packer.stitch();
  auto atlasWidth = static_cast<float>(m_Atlas->getWidth());
  auto atlasHeight = static_cast<float>(m_Atlas->getHeight());

  m_BlockDataBuffer = context.allocateUniformBuffer(sizeof(BlockData) * 256);

  auto buffer = std::make_unique<BlockData[]>(sizeof(BlockData) * 256);
  for (size_t i = 0; i < m_Blocks.size(); i++) {
    if (i == 0) {
      buffer[i] = BlockData{glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)};
      continue;
    }
    auto& container = textureHandles[i - 1];
    auto* texture = container.texture;

    auto x = static_cast<float>(texture->x) / atlasWidth;
    auto y = static_cast<float>(texture->y) / atlasHeight;
    auto w = static_cast<float>(texture->width) / atlasWidth;
    auto h = static_cast<float>(texture->height) / atlasHeight;

    buffer[i] = BlockData{glm::vec4(x, y, w, h)};
    container.block->m_AtlasPosition = glm::vec4(x, y, w, h);
  }

  m_BlockDataBuffer->write(0, buffer.get(), sizeof(BlockData) * 256);
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