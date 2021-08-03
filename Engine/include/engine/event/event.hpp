#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_EVENT_EVENT_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_EVENT_EVENT_HPP_

#include <memory>

namespace engine {

namespace render {
class RenderContext;
}

template <typename T>
class Event {
public:
    Event(render::RenderContext& context);

    render::RenderContext& getContext() { return *m_Owner; }
private:
    render::RenderContext* m_Owner;
};

template <typename T>
class _EventRegistry {

};

template <typename T>
class EventConsumer {
public:
    virtual void consumeEvent(T& event) = 0;
};

class EventDispatcher {
public:
    template <typename T>
    void registerConsumer(const EventConsumer<T>) {}

    template <typename T>
    void dispatch(T& event);
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_EVENT_EVENT_HPP_
