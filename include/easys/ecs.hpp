#pragma once

#include "entity.hpp"
#include "registry.hpp"
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <set>

class ECS {
  public:
	ECS()
	{
		for (Entity entity = 0; entity < MAX_ENTITIES; entity++)
			availableEntityIds_.push(entity);
	}

	// This constructor is for initialising the ECS with a set of specific entities. I decided against an
	// addEntity(Entity) method to discourage tampering with entities too much. I think this really should be the ECS's
	// responsibility.
	ECS(const std::set<Entity> &oldEntities)
	{
		for (Entity entity = 0; entity < MAX_ENTITIES; entity++)
			if (oldEntities.contains(entity))
				entities_.insert(entity);
			else
				availableEntityIds_.push(entity);
	}

	Entity addEntity()
	{
		if (getEntityCount() < MAX_ENTITIES) {
			Entity e = availableEntityIds_.front();
			availableEntityIds_.pop();
			entities_.insert(e);
			return e;
		}
		// throwing an exception here seems kind of drastic, but on the other hand maybe not
		throw std::runtime_error("MAX NUMBER OF ENTITIES REACHED!");
	}

	void removeEntity(const Entity e)
	{
		// Remove all components associated with the entity
		registry_.removeComponents(e);
		// Remove entity from the set of active entities_
		entities_.erase(e);
		// Make the entity ID available again
		availableEntityIds_.push(e);
	}

	bool hasEntity(const Entity e) const { return entities_.contains(e); }

	const std::set<Entity> &getEntities() const { return entities_; }

	size_t getEntityCount() const { return entities_.size(); }

	template <typename T>
	void addComponent(const Entity e, const T c)
	{
		registry_.addComponent(e, c);
	}

	template <typename T>
	void removeComponent(const Entity e)
	{
		registry_.removeComponent<T>(e);
	}

	template <typename T>
	T &getComponent(const Entity e)
	{
		return registry_.getComponent<T>(e);
	}

	template <typename T>
	bool hasComponent(const Entity e) const
	{
		return registry_.hasComponent<T>(e);
	}

	template <typename T>
	const std::vector<Entity> &getEntitiesByComponent() const
	{
		return registry_.getEntitiesByComponent<T>();
	}

	template <typename... Ts>
	const std::vector<Entity> getEntitiesByComponents() const
	{
		return registry_.getEntitiesByComponents<Ts...>();
	}

	template <typename T>
	std::vector<T> &getComponentsByType()
	{
		return registry_.getComponentsByType<T>();
	}

	size_t getComponentCount() const { return registry_.size(); }

	template<typename... ComponentTypes>
	void clear()
	{
		registry_.clear();
		entities_.clear();

		std::queue<Entity> empty;
		std::swap(availableEntityIds_, empty);

		for (Entity entity = 0; entity < MAX_ENTITIES; entity++)
			availableEntityIds_.push(entity);
	}
  private:
	std::queue<Entity> availableEntityIds_;
	std::set<Entity> entities_;
	Registry registry_;
};