#pragma once

#include "Entity.hpp"
#include "VectorRegistry.hpp"
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <set>

class ECSManager {
  public:
	ECSManager()
	{
		for (Entity entity = 0; entity < MAX_ENTITIES; entity++)
			availableEntityIds.push(entity);
	}

	// This constructor is for initialising the ECS with a set of specific entities. I decided against an
	// addEntity(Entity) method to discourage tampering with entities too much. I think this really should be the ECS's
	// responsibility.
	ECSManager(const std::set<Entity> &oldEntities)
	{
		for (Entity entity = 0; entity < MAX_ENTITIES; entity++)
			if (oldEntities.contains(entity))
				entities.insert(entity);
			else
				availableEntityIds.push(entity);
	}

	Entity addEntity()
	{
		if (getEntityCount() < MAX_ENTITIES) {
			Entity e = availableEntityIds.front();
			availableEntityIds.pop();
			entities.insert(e);
			return e;
		}
		// throwing an exception here seems kind of drastic, but on the other hand maybe not
		throw std::runtime_error("MAX NUMBER OF ENTITIES REACHED!");
	}

	void removeEntity(const Entity e)
	{
		// Remove all components associated with the entity
		registry.removeComponents(e);

		// Remove entity from the set of active entities
		entities.erase(e);

		// Make the entity ID available again
		availableEntityIds.push(e);
	}

	bool hasEntity(const Entity e) const { return entities.contains(e); }

	template <typename T>
	void addComponent(const Entity e, const T c)
	{
		registry.addComponent(e, c);
	}

	template <typename T>
	void removeComponent(const Entity e)
	{
		registry.removeComponent<T>(e);
	}

	template <typename T>
	T &getComponent(const Entity e) // not const, because: see registry.getComponent
	{
		return registry.getComponent<T>(e);
	}

	template <typename T>
	std::vector<std::reference_wrapper<T>> getAllComponentsByType() // not const, because: see registry.getComponent
	{
		return registry.getAllComponentsByType<T>();
	}

	template <typename T>
	bool hasComponent(const Entity e) const
	{
		return registry.hasComponent<T>(e);
	}

	const std::set<Entity> &getEntities() const { return entities; }

	size_t getEntityCount() const { return entities.size(); }
	size_t getComponentCount() const { return registry.size(); }

  private:
	std::queue<Entity> availableEntityIds;
	std::set<Entity> entities;
	Registry registry;
};