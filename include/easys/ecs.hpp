#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <set>

#include "entity.hpp"
#include "registry.hpp"

namespace Easys {

template <typename... AllComponentTypes>
class ECS {
   public:
	ECS()
	{
		for (Entity entity = 0; entity < MAX_ENTITIES; entity++)
		{
			availableEntityIds_.push(entity);
		}
	}

	// This constructor is for initialising the ECS with a set of specific
	// entities. I decided against an addEntity(Entity) method to discourage
	// tampering with entities too much. I think this really should be the ECS's
	// responsibility.
	ECS(const std::set<Entity>& oldEntities)
	{
		for (Entity entity = 0; entity < MAX_ENTITIES; entity++)
		{
			if (oldEntities.contains(entity))
				entities_.insert(entity);
			else
				availableEntityIds_.push(entity);
		}
	}

	inline Entity addEntity()
	{
		if (getEntityCount() < MAX_ENTITIES)
		{
			Entity e = availableEntityIds_.front();
			availableEntityIds_.pop();
			entities_.insert(e);
			return e;
		}
		// throwing an exception here seems kind of drastic, but on the other hand
		// maybe not
		throw std::runtime_error("MAX NUMBER OF ENTITIES REACHED!");
	}

	inline void removeEntity(const Entity e)
	{
		// Remove all components associated with the entity
		registry_.removeComponents(e);
		// Remove entity from the set of active entities_
		entities_.erase(e);
		// Make the entity ID available again
		availableEntityIds_.push(e);
	}

	inline bool hasEntity(const Entity e) const { return entities_.contains(e); }

	inline const std::set<Entity>& getEntities() const { return entities_; }

	inline size_t getEntityCount() const { return entities_.size(); }

	template <typename T>
	inline void addComponent(const Entity e, T component)
	{
		registry_.addComponent(e, std::move(component));
	}

	template <typename T>
	inline void removeComponent(const Entity e)
	{
		registry_.template removeComponent<T>(e);
	}

	template <typename T>
	inline T& getComponent(const Entity e)
	{
		return registry_.template getComponent<T>(e);
	}

	template <typename T>
	inline const T& getComponent(const Entity e) const
	{
		return registry_.template getComponent<T>(e);
	}

	template <typename T>
	inline bool hasComponent(const Entity e) const
	{
		return registry_.template hasComponent<T>(e);
	}

	template <typename T>
	inline const std::vector<Entity>& getEntitiesByComponent() const
	{
		return registry_.template getEntitiesByComponent<T>();
	}

	template <typename... Ts>
	inline std::vector<Entity> getEntitiesByComponents() const
	{
		return registry_.template getEntitiesByComponents<Ts...>();
	}

	inline size_t getComponentCount() const { return registry_.size(); }

	template <typename... Ts>
	inline size_t getComponentCount() const
	{
		return registry_.template size<Ts...>();
	}

	inline void clear()
	{
		registry_.clear();
		clearEntities();
	}

	inline void clearComponents() { registry_.clear(); }

	template <typename... Ts>
	inline void clearComponents()
	{
		registry_.template clear<Ts...>();
	}

   private:
	std::queue<Entity> availableEntityIds_;
	std::set<Entity> entities_;
	Registry<AllComponentTypes...> registry_;

	void clearEntities()
	{
		entities_.clear();

		std::queue<Entity> empty;
		std::swap(availableEntityIds_, empty);

		for (Entity entity = 0; entity < MAX_ENTITIES; entity++) availableEntityIds_.push(entity);
	}
};

}  // namespace Easys