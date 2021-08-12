#include "world.hpp"

#include "engine/file.hpp"

#include "engine/nbt/nbt_type.hpp"

namespace mc {

using namespace engine::nbt;

World::World(std::string name) {
  engine::File worldInfo("saves/");

  NbtCompound compound;
  compound["test"] = 1.0f;

  NbtList list;
  list = NbtType::TAG_FLOAT;

  list.pushBack((NbtValue() = NbtType::TAG_FLOAT) = 1.0f);

  compound["testList"] = list;
}

}