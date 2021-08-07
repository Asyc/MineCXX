#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_EVENT_EVENT_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_EVENT_EVENT_HPP_

#include <functional>
#include <memory>

namespace engine {

namespace render {
class RenderContext;
}

template<typename T>
class Event {
 public:
  Event(render::RenderContext& context);

  render::RenderContext& getContext() { return *m_Owner; }
 private:
  render::RenderContext* m_Owner;
};

template<typename T>
class EventRegistry {
 public:

};

class EventDispatcher {
 public:
  template<typename T>
  void registerCallback(std::function<Event<T>> consumer) {}

  template<typename T>
  void dispatch(T& event);
 private:
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_EVENT_EVENT_HPP_
