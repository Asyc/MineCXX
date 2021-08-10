#include "chunk.hpp"

#include <glm/glm.hpp>

extern engine::render::RenderContext* g_RenderContext;

namespace mc {

Chunk::Chunk(int32_t x, int32_t y) : m_X(x), m_Y(y) {
  struct Vertex {
    glm::vec3 pos;
    glm::vec2 texPos;
  };

  m_Pipeline = g_RenderContext->createRenderPipeline(engine::File("assets/shaders/world/standard/"));

  std::array<Vertex, 24> blockModel = {
      Vertex{glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
      Vertex{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
      Vertex{glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
      Vertex{glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},

      Vertex{glm::vec3(0.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
      Vertex{glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
      Vertex{glm::vec3(1.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)},
      Vertex{glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},

      Vertex{glm::vec3(0.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
      Vertex{glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
      Vertex{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
      Vertex{glm::vec3(0.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},

      Vertex{glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
      Vertex{glm::vec3(1.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
      Vertex{glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
      Vertex{glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},

      Vertex{glm::vec3(0.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
      Vertex{glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
      Vertex{glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
      Vertex{glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},

      Vertex{glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
      Vertex{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
      Vertex{glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
      Vertex{glm::vec3(1.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
  };

  std::array<uint32_t, 6> indices = {
      0, 1, 2, 0, 3, 2,
  };

  std::array<uint32_t, 6 * 6> fullIndices{};
  for (size_t i = 0; i < 6; i++) {
    for (size_t j = 0; j < 6; j++) {
      fullIndices[(i * 6) + j] = indices[j] + (i * 4);
    }
  }

  m_BlockVbo = g_RenderContext->allocateVertexBuffer(sizeof(blockModel));
  m_BlockIbo = g_RenderContext->allocateIndexBuffer(sizeof(fullIndices));

  m_BlockVbo->write(0, blockModel.data(), sizeof(blockModel));
  m_BlockIbo->write(0, fullIndices.data(), sizeof(fullIndices));

  m_BlockData = {};
}

void Chunk::setBlock(int32_t x, int32_t y, int32_t z, Block* blockType) {
  m_BlockData[x][y][z] = BlockContainer{blockType};

}

void Chunk::update() {

}

struct BlockData {
  glm::vec3 position;
  int32_t blockId;
};

void Chunk::tick() {
  auto worldBuffer = g_RenderContext->allocateUniformBuffer(sizeof(int32_t) + sizeof(BlockData) * 16 * 256 * 16);

  m_UniformDescriptor = m_Pipeline->allocateDescriptorSet(0);
  m_UniformDescriptor->bind(0, *g_RenderContext->getViewport().getUniformBuffer());
  m_UniformDescriptor->bind(1, *mc::BlockRegistry::getInstance().getBlockRegistryBuffer());
  m_UniformDescriptor->bind(3, *mc::BlockRegistry::getInstance().getAtlas());

  int32_t count = 0;

  for (size_t x = 0; x < 16; x++) {
    for (size_t y = 0; y < 256; y++) {
      for (size_t z = 0; z < 16; z++) {
        auto& block = m_BlockData[x][y][z];
        if (block.blockType != nullptr) {
          size_t index = count++;
          BlockData data{glm::vec3(x, y, z), (int32_t) block.blockType->getId()};
          worldBuffer->write(count * sizeof(BlockData), &data, sizeof(BlockData));
        }
      }
    }
  }

  m_UniformDescriptor->bind(2, *worldBuffer);
  m_BlockCount = count;
}

void Chunk::draw(engine::IDrawableCommandBuffer& commandBuffer) {
  if (m_UniformDescriptor == nullptr) tick();
  commandBuffer.bindPipeline(*m_Pipeline);
  commandBuffer.bindVertexBuffer(*m_BlockVbo);
  commandBuffer.bindIndexBuffer(*m_BlockIbo);
  commandBuffer.bindUniformDescriptor(*m_UniformDescriptor);
  commandBuffer.drawIndexed(m_BlockCount, 6 * 6);
}

}