#ifndef MINECRAFTCXX_CLIENT_GAME_SRC_WORLD_WORLD_OBJECT_HPP_
#define MINECRAFTCXX_CLIENT_GAME_SRC_WORLD_WORLD_OBJECT_HPP_

#include "engine/render/command/command_buffer.hpp"

namespace engine {

using render::command::IDrawableCommandBuffer;

class WorldObject {
 public:
  virtual ~WorldObject() = default;

  /**
   * Called every frame
   *
   * @param deltaTicks A value between (0.0 - 1.0) that represents the time passed since the last tick, in ticks.
   */
  virtual void update(float deltaTicks) = 0;  // Called each frame

  /**
   * Called every (1/20) of a second
   */
  virtual void tick() = 0;

  /**
   * Records the render commands needed to render this object.
   *
   * @param commandBuffer The command buffer render commands are recorded to
   */
  virtual void draw(IDrawableCommandBuffer& commandBuffer) = 0;
};

}

#endif //MINECRAFTCXX_CLIENT_GAME_SRC_WORLD_WORLD_OBJECT_HPP_
