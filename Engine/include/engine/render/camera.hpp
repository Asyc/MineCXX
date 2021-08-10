#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_CAMERA_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_CAMERA_HPP_

#include <glm/glm.hpp>

namespace engine::render {

class Camera {
 public:
 private:
  float m_FOV;
  glm::vec3 m_CameraPos;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_CAMERA_HPP_
