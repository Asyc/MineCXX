#ifndef MINECRAFTCXX_CLIENT_GAME_SRC_WORLD_CHUNK_HPP_
#define MINECRAFTCXX_CLIENT_GAME_SRC_WORLD_CHUNK_HPP_

#include <array>

namespace mc {

class Chunk {
 public:
  struct BlockContainer {};
 private:
  std::array<BlockContainer, 16 * 16 * 256> m_BlockData;
};

}

#endif //MINECRAFTCXX_CLIENT_GAME_SRC_WORLD_CHUNK_HPP_
