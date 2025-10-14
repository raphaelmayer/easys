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

// TODO: We could make CallbackId more complex, where the subscription only exists as long as the object is still alive, 
// i.e. we could cleanup in the dtor, so the user would not have to unsubscribe. 
// But is this an advantage? Then you would also need to keep the "connection" alive. 
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
		std::function<void(const Event&)> wrapper = std::forward<Func>(fn);
		vec.push_back(Subscriber<Event>{id, std::move(wrapper)});
		return id;
	}

	// Subscribe to an event for a specific entity.
	template <typename Event, typename Func>
	CallbackId subscribe(Entity e, Func&& fn)
	{
		// TODO: add subscriber to list of subscribers for a specific entity. 
		// does this even make sense? should we let the user handle the "filtering"?
	
		// 1: Would be a cheap solution and only pretends to do the thing it should. 
		// The sub would still get ALL events of type Event, but would reject them, if they are not from the entity in question.
		auto wrapFn = [&](const Event& ev)
		{ 
			if (ev.entity == e)
				fn(); 
		};
		return subscribe<Event>(wrapFn);
		
		// 2. Another solution would be to delegate this "filtering" to dispatch, where we call callbacks only if the event is from a subscribed to entity. 
		// But then we also need to track, which entities (if any single one) are being tracked by which sub. Possibly in Subscriber?
	}

	// Remove a subscription from the list of subscribers.
	void unsubscribe(CallbackId id)
	{
		if (id == 0) return;
		unsubscribe_impl(id, std::index_sequence_for<Events...>{});
	}

	template <typename Event>
	void emit(Event ev)
	{
		static_assert(contains<Event>(), "Event not registered in EventBus<Event...>");
		auto& q = queue_for<Event>();
		q.push_back(std::move(ev));
	}

	// Process all pending events.
	void dispatch() { dispatch_impl(std::index_sequence_for<Events...>{}); }

	// reserve helpers to avoid allocations: call from init
	// TODO: do we want/need this?
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
	CallbackId nextId_ = 1;

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

	// unsubscribe implementation: iterate event types and remove id if found
	template <std::size_t... I>
	void unsubscribe_impl(CallbackId id, std::index_sequence<I...>)
	{
		// fold-expression to call removal for each tuple entry. can we simplify?
		(remove_from_vector(std::get<I>(subscribers_), id), ...);
	}

	template <typename SubVec>
	static void remove_from_vector(SubVec& vec, CallbackId id)
	{
		auto it = std::find_if(vec.begin(),
		                       vec.end(),
		                       [id](const auto& s)
		                       {
			                       return s.id == id;
		                       });
		if (it != vec.end())
		{
			// fast remove: swap with back and pop_back
			std::swap(*it, vec.back());
			vec.pop_back();
		}
	}

	// dispatch implementation: for each event type, snapshot queue, clear it, snapshot subscribers, call them
	template <std::size_t... I>
	void dispatch_impl(std::index_sequence<I...>)
	{
		// expand over event types
		(dispatch_one<Events>(std::get<I>(queues_), std::get<I>(subscribers_)), ...);
	}

	template <typename Event>
	void dispatch_one(std::vector<Event>& queue, std::vector<Subscriber<Event>>& subs)
	{
		if (queue.empty() || subs.empty())
		{
			queue.clear();
			return;
		}

		// TODO: snapshotting is kind of expensive. do we need this or could we solve this somehow?
		// move queue out to process without holding it (minimize time with queue active)
		std::vector<Event> processing;
		processing.swap(queue);

		// snapshot subscribers to protect against subscribe/unsubscribe during callbacks
		std::vector<Subscriber<Event>> subs_snapshot = subs;

		for (const auto& ev : processing)
		{
			for (const auto& s : subs_snapshot)
			{
				s.cb(ev);
			}
		}
	}
};

};  // namespace Easys