#pragma once

#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "entity.hpp"

namespace Easys {

struct EntityAdded {
	Entity entity;
};

template <typename Component>
struct EntityUpdated {
	Entity entity;
	Component component;
};

struct EntityRemoved {
	Entity entity;
};

template <typename Component>
struct ComponentAdded {
	Entity entity;
	Component component;
};

template <typename Component>
struct ComponentAccessed {
	Entity entity;
	Component component;
};

template <typename Component>
struct ComponentUpdated {
	Entity entity;
	Component component;
};

template <typename Component>
struct ComponentRemoved {
	Entity entity;
	Component component;
};

using CallbackId = uint64_t;

template <typename... Events>
class Eventbus {
   public:
	Eventbus() = default;
	~Eventbus() = default;

	// Subscribe to an event.
	template <typename Event, typename Func>
	CallbackId subscribe(Func&& fn)
	{
		static_assert(contains<Event>(), "Event not registered in EventBus<Event...>");
		auto& vec = subscribers_for<Event>();
		CallbackId id = ++nextId_;
		// wrap callable: accept anything convertible to void(const Event&)
		// we could handle this differently, e.g. a small custom wrapper for the functions.
		std::function<void(const Event&)> wrapper = std::forward<Func>(f);
		vec.push_back(Subscriber<Event>{id, std::move(wrapper)});
		return id;
	}

	// Subscribe to an event for a specific entity.
	template <typename Event, typename Func>
	CallbackId subscribe(Entity e, Func&& fn)
	{
		// TODO: add subscriber to list of subscribers for a specific entity
	}

	// Remove a subscription from the list of subscribers.
	void unsubscribe(CallbackId id)
	{
		// TODO
	}

	template <typename Event>
	void emit(Event event)
	{
		static_assert(contains<Event>(), "Event not registered in EventBus<Event...>");
		auto& q = queue_for<Event>();
		q.push_back(std::move(ev));
	}

	// Process all pending events.
	void dispatch() { 
		// TODO
	}

	// reserve helpers to avoid allocations: call from init
	// TODO: do we want this?
	template <typename Event>
	void reserve_subscribers(size_t n)
	{
		subscribers_for<Event>().reserve(n);
	}
	template <typename Event>
	void reserve_queue(size_t n)
	{
		queue_for<Event>().reserve(n);
	}

   private:
	template <typename Event>
	struct Subscriber {
		CallbackId id;
		std::function<void(const Event&)> cb;
	};

	std::tuple<std::vector<Events>...> queues_;
	std::tuple<std::vector<Subscriber<Events>>...> subscribers_;  // is this necessary?
	// std::vector<Subscriber> subs_;  // orcould we do something like this?
	CallbackId nextId = 1;

	// helpers
	template <typename Event>
	static constexpr bool contains()
	{
		return (std::is_same<Event, Events>{} || ...);
	}

	template <typename Event>
	auto& subscribers_for()
	{
		return std::get<std::vector<Subscriber<Event>>>(subscribers_);
	}

	template <typename Event>
	auto& queue_for()
	{
		return std::get<std::vector<Event>>(queues_);
	}
};

void test()
{
	struct Position {
		float x, y;
	};

	Eventbus<EntityAdded> eb;
	Entity e{0};
	Position p{1, 2};

	CallbackId id = eb.subscribe<EntityAdded>(e, [](EntityAdded ev) {});
	CallbackId id2 = eb.subscribe<EntityAdded>(e, [](EntityAdded ev) {});

	eb.unsubscribe(id2);

	// eb.emit<ComponentAdded<Position>>(e, p);
}

};  // namespace Easys