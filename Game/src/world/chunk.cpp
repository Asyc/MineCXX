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
      Vertex{glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},

      Vertex{glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
      Vertex{glm::vec3(1.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
      Vertex{glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
      Vertex{glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},

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
  for (uint32_t i = 0; i < 6; i++) {
    for (uint32_t j = 0; j < 6; j++) {
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

void Chunk::update(float deltaTicks) {

}

struct BlockData {
  glm::vec3 position;
  int32_t blockId;
};

void Chunk::tick() {
  if (m_UniformDescriptor != nullptr) return;
  m_StorageBuffer = g_RenderContext->allocateStorageBuffer(sizeof(BlockData) * 16 * 256 * 16);
  m_UniformDescriptor = m_Pipeline->allocateDescriptorSet(0);
  m_UniformDescriptor->bind(0, *g_RenderContext->getCamera().getBuffer());
  m_UniformDescriptor->bind(1, *mc::BlockRegistry::getInstance().getBlockRegistryBuffer());
  m_UniformDescriptor->bind(3, *mc::BlockRegistry::getInstance().getAtlas());

  int32_t count = 0;

  auto buffer = std::make_unique<BlockData[]>(16 * 256 * 16);

  for (size_t x = 0; x < 16; x++) {
    for (size_t y = 0; y < 256; y++) {
      for (size_t z = 0; z < 16; z++) {
        auto& block = m_BlockData[x][y][z];
        if (block.blockType != nullptr) {
          auto fx = static_cast<float>(x);
          auto fy = static_cast<float>(y);
          auto fz = static_cast<float>(z);
          BlockData data{glm::vec4(fx, fy, fz, 0.0f), (int32_t) block.blockType->getId()};
          buffer[count++] = data;
        }
      }
    }
  }

  m_StorageBuffer->write(0, buffer.get(), sizeof(BlockData) * count);
  m_UniformDescriptor->bind(2, *m_StorageBuffer);
  m_BlockCount = count;
}

void Chunk::draw(engine::IDrawableCommandBuffer& commandBuffer) {
  if (m_UniformDescriptor == nullptr) tick();
  commandBuffer.bindPipeline(*m_Pipeline);
  commandBuffer.bindVertexBuffer(*m_BlockVbo);
  commandBuffer.bindIndexBuffer(*m_BlockIbo);
  commandBuffer.bindUniformDescriptor(*m_UniformDescriptor);
  commandBuffer.drawIndexed(m_BlockCount, 36);
}

}