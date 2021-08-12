#ifndef MINECRAFTCXX_CLIENT_GAME_SRC_WORLD_WORLD_HPP_
#define MINECRAFTCXX_CLIENT_GAME_SRC_WORLD_WORLD_HPP_

#include "chunk.hpp"

#include "engine/world_object.hpp"

namespace mc {

class World : public engine::WorldObject {
 public:
  World(std::string name);

  const std::string& getName() const {return name;}
 private:
  std::string name;
};

}

#endif //MINECRAFTCXX_CLIENT_GAME_SRC_WORLD_WORLD_HPP_
