#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_DESCRIPTOR_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_DESCRIPTOR_HPP_

#include <cstdint>

namespace engine::render {

class IDescriptorResource {
 public:
  virtual ~IDescriptorResource() = default;
};

class UniformDescriptor {
 public:
  virtual ~UniformDescriptor() = default;

  virtual void bind(uint32_t binding, const IDescriptorResource& buffer) = 0;
  virtual void bindAll(uint32_t* bindings, const IDescriptorResource* buffers, size_t count) = 0;
 protected:
  UniformDescriptor() = default;
};

}   // namespace engine::render

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_RENDER_DESCRIPTOR_HPP_
