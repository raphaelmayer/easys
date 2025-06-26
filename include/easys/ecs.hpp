#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <set>

#include "entity.hpp"
#include "registry.hpp"

namespace Easys {

/**
 * @brief Manages entities and components in an Entity-Component-System architecture.
 * @tparam AllComponentTypes A list of all possible component types that can be used in this ECS instance.
 */
template <typename... AllComponentTypes>
class ECS {
   public:
	/**
	 * @brief Initializes the ECS with a predefined maximum number of entities (MAX_ENTITIES).
	 * @details All entity IDs are initially available for assignment.
	 */
	ECS()
	{
		for (Entity entity = 0; entity < MAX_ENTITIES; entity++)
		{
			availableEntityIds_.push(entity);
		}
	}

	/**
	 * @brief Initializes the ECS with a specific set of entities.
	 * @details This constructor is useful for creating a new ECS instance based on a subset
	 * of entities from another instance or a predefined list.
	 * @param entities A set of entities to initialize the ECS with.
	 */
	ECS(const std::set<Entity>& oldEntities)
	{
		// I decided against an addEntity(Entity) method to discourage
		//  tampering with entities too much. I think this really should be the ECS's
		//  responsibility.
		for (Entity entity = 0; entity < MAX_ENTITIES; entity++)
		{
			if (oldEntities.contains(entity))
				entities_.insert(entity);
			else
				availableEntityIds_.push(entity);
		}
	}

	/**
	 * @brief Adds a new entity to the ECS.
	 * @return The ID of the newly created entity.
	 * @throws std::runtime_error if the maximum number of entities (MAX_ENTITIES) is reached.
	 */
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

	/**
	 * @brief Removes an entity and all its associated components from the ECS.
	 * @details The removed entity's ID is made available for reuse.
	 * @param e The entity to remove.
	 */
	inline void removeEntity(const Entity e)
	{
		// Remove all components associated with the entity
		registry_.removeComponents(e);
		// Remove entity from the set of active entities_
		entities_.erase(e);
		// Make the entity ID available again
		availableEntityIds_.push(e);
	}

	/**
	 * @brief Checks if an entity exists within the ECS.
	 * @param e The entity to check for.
	 * @return True if the entity exists, false otherwise.
	 */
	inline bool hasEntity(const Entity e) const { return entities_.contains(e); }

	/**
	 * @brief Returns a reference to the set of all entities.
	 * @return A constant reference to the set of all entities currently in the ECS.
	 */
	inline const std::set<Entity>& getEntities() const { return entities_; }

	/**
	 * @brief Returns a vector of entities that have a component of a specific type.
	 * @tparam T The component type to query for.
	 * @return A constant reference to a vector of entities possessing the component.
	 */
	template <typename T>
	inline const std::vector<Entity>& getEntitiesByComponent() const
	{
		return registry_.template getEntitiesByComponent<T>();
	}

	/**
	 * @brief Returns a vector of entities that have all of the specified component types.
	 * @tparam Ts A variadic list of component types to query for.
	 * @return A vector of entities that possess all specified components.
	 */
	template <typename... Ts>
	inline std::vector<Entity> getEntitiesByComponents() const
	{
		return registry_.template getEntitiesByComponents<Ts...>();
	}

	/**
	 * @brief Returns the total number of active entities in the ECS.
	 * @return The number of entities.
	 */
	inline size_t getEntityCount() const { return entities_.size(); }

	/**
	 * @brief Adds a component of type T to an entity.
	 * @details If the entity already has a component of type T, it will be updated with the new value.
	 * @tparam T The type of the component to add.
	 * @param e The entity to which the component will be added.
	 * @param c The component data to add.
	 */
	template <typename T>
	inline void addComponent(const Entity e, T component)
	{
		registry_.addComponent(e, std::move(component));
	}

	/**
	 * @brief Removes a component of type T from an entity.
	 * @tparam T The type of the component to remove.
	 * @param e The entity from which to remove the component.
	 */
	template <typename T>
	inline void removeComponent(const Entity e)
	{
		registry_.template removeComponent<T>(e);
	}

	/**
	 * @brief Retrieves a reference to a component of type T from an entity.
	 * @tparam T The type of the component to retrieve.
	 * @param e The entity whose component is to be retrieved.
	 * @return A mutable reference to the component.
	 */
	template <typename T>
	inline T& getComponent(const Entity e)
	{
		return registry_.template getComponent<T>(e);
	}

	/**
	 * @brief Retrieves a reference to a component of type T from an entity.
	 * @tparam T The type of the component to retrieve.
	 * @param e The entity whose component is to be retrieved.
	 * @return A immutable reference to the component.
	 */
	template <typename T>
	inline const T& getComponent(const Entity e) const
	{
		return registry_.template getComponent<T>(e);
	}

	/**
	 * @brief Checks if an entity has a component of type T.
	 * @tparam T The type of the component to check for.
	 * @param e The entity to check.
	 * @return True if the entity has the component, false otherwise.
	 */
	template <typename T>
	inline bool hasComponent(const Entity e) const
	{
		return registry_.template hasComponent<T>(e);
	}

	/**
	 * @brief Returns the total count of components of the specified types within the ECS.
	 * @tparam Ts A variadic list of component types. If template parameters are omitted, it returns the total count of
	 * all component types.
	 * @return The total number of components of the specified types.
	 */
	template <typename... Ts>
	inline size_t getComponentCount() const
	{
		return registry_.template size<Ts...>();
	}

	inline size_t getComponentCount() const { return registry_.size(); }

	/**
	 * @brief Clears all entities and components from the ECS.
	 * @details Resets the ECS to its initial state, making all entity IDs available again.
	 */
	inline void clear()
	{
		registry_.clear();
		clearEntities();
	}

	/**
	 * @brief Removes components of specific types from all entities within the ECS.
	 * @tparam Ts A variadic list of component types to clear. If template parameters are omitted, all types of
	 * components are cleared from all entities.
	 */
	template <typename... Ts>
	inline void clearComponents()
	{
		registry_.template clear<Ts...>();
	}

	inline void clearComponents() { registry_.clear(); }

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