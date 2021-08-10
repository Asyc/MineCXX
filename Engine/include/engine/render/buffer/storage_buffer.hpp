#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_BUFFER_STORAGE_BUFFER_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_BUFFER_STORAGE_BUFFER_HPP_

#include <cstdint>

#include "engine/render/descriptor.hpp"

namespace engine::render::buffer {

class StorageBuffer : public IDescriptorResource {
 public:
  virtual ~StorageBuffer() = default;

  virtual void write(size_t offset, const void* ptr, size_t length) = 0;
 protected:
  StorageBuffer() = default;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_BUFFER_STORAGE_BUFFER_HPP_
