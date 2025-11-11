#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <set>

#include "entity.hpp"
#include "eventbus.hpp"
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
		// tampering with entities too much. I think this really should be the ECS's
		// responsibility.
		for (Entity entity = 0; entity < MAX_ENTITIES; entity++)
		{
			if (oldEntities.contains(entity))
			{
				entities_.insert(entity);
				eventbus_.template emit<EntityAdded>({entity});
			}
			else
			{
				availableEntityIds_.push(entity);
			}
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
			eventbus_.template emit<EntityAdded>({e});
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
		eventbus_.template emit<EntityRemoved>({e});
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
	 * @brief Returns a vector of entities that have all of the specified component types. Use smaller components first
	 * for optimal performance.
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
		eventbus_.template emit<ComponentAdded<T>>({e, component});
		eventbus_.template emit<EntityUpdated<T>>({e, component});
		registry_.addComponent(e, std::move(component));
	}

	/**
	* @brief Modifies a component of type T for a given entity.
	* @details This function retrieves the component of type T associated with the specified entity
	* and invokes the provided callable function with a reference to that component. The callable can
	* be a lambda, function pointer, or any other callable type. The component can be modified directly
	* through the callable.
	* @tparam T The type of the component to modify.
	* @param e The entity whose component will be modified.
	* @param fn The callable that will be used to modify the component. It should accept a reference to
	*           the component of type T.
	*/
	template <typename T, typename Func>
	inline void modifyComponent(const Entity e, Func&& fn)
	{
		// TODO: should we check component existence?
		T& c = getComponent<T>(e);
		fn(c);
		eventbus_.template emit<ComponentUpdated<T>>({e, c});
		eventbus_.template emit<EntityUpdated<T>>({e, c});
		// classic way of forwarding ops to registry. kind of falls apart when wanting to fire events and looping over multiple types. 
		// registry_.template modifyComponent<T>(e, fn); 
	}

	/**
	* @brief Modifies a component of type T for a given entity.
	* @details This function retrieves the component of type T associated with the specified entity
	* and replaces it with the new component provided as the argument c.
	* @tparam T The type of the component to modify.
	* @param e The entity whose component will be modified.
	* @param c The new component of type T that will replace the existing component.
	*/
	template <typename T>
	inline void modifyComponent(const Entity e, T c)
	{
		// TODO: should we check component existence?
		getComponent<T>(e) = c;
		eventbus_.template emit<ComponentUpdated<T>>({e, c});
		eventbus_.template emit<EntityUpdated<T>>({e, c});
		// classic way of forwarding ops to registry. kind of falls apart when wanting to fire events and looping over multiple types. 
		// registry_.template modifyComponent<T>(e, c); 
	}

	/**
	 * @brief Removes a component of type T from an entity.
	 * @tparam T The type of the component to remove.
	 * @param e The entity from which to remove the component.
	 */
	template <typename T>
	inline void removeComponent(const Entity e)
	{
		const T& c = getComponent<T>(e);
		eventbus_.template emit<ComponentRemoved<T>>({e, c});
		eventbus_.template emit<EntityUpdated<T>>({e, c});
		registry_.template removeComponent<T>(e);
		// I wonder if we dont just want to have the variadic template versions and 
		// just use those for all calls, i.e. with single T, SomeTypes and AllTypes.
		// registry_.template removeComponents<T>(e);
	}

	/**
	 * @brief Removes all components from an entity.
	 * @param e The entity from which to remove all components.
	 */
	inline void removeComponents(const Entity e) { 
		// registry_.forEachComponentType<AllComponentTypes>([]<typename T>(){
		// 	T& c = getComponent<T>(e);
		// 	eventbus_.template emit<ComponentRemoved<T>>({e, c});
		// });
		// registry_.removeComponents(e); 

		// we definitely dont want to put event handling in the registry. 
		// 1. would be a possible solution. now we dont use registry.forEachComponentType at all. 
		// 2. we could also make registry.forEach... public and use this instead. 
		// ([&](){
		// 	eventbus_.template emit<ComponentRemoved<AllComponentTypes>>({e, getComponent<AllComponentTypes>(e)});
		// 	registry_.template removeComponent<AllComponentTypes>(e);
		// }(), ...);
		
		// We could just use the templated function...
		// removeComponents<AllComponentTypes>(e);

		// or something like this and get event dispatch for free
		(removeComponent<AllComponentTypes>(e), ...);
	}

	/**
	 * @brief Removes all components of types Ts from an entity.
	 * @tparam T The types of the components to remove.
	 * @param e The entity from which to remove the components.
	 */
	template <typename... Ts>
	inline void removeComponents(const Entity e)
	{
		// registry_.template removeComponents<Ts...>(e);
		// ([&](){
		// 	eventbus_.template emit<ComponentRemoved<Ts>>({e, getComponent<Ts>(e)});
		// 	registry_.template removeComponent<Ts>(e);
		// }(), ...);

		// same here
		(removeComponent<Ts>(e), ...);
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

	inline void clearComponents() 
	{ 
		// registry_.clear(); // forward
		// registry_.template clear<AllComponentTypes...>();
		// so we need the variadic version on ecs level, hence if we want the convenient all-version, we would just do it as follows:
		clearComponents<AllComponentTypes...>();

	}

   private:
	std::queue<Entity> availableEntityIds_;
	std::set<Entity> entities_;
	Registry<AllComponentTypes...> registry_;

	// Define the event types based on the provided component types
    using ALL_EVENT_TYPES = std::tuple<
        EntityAdded,
		EntityUpdated<AllComponentTypes>...,
        EntityRemoved,
        ComponentAdded<AllComponentTypes>...,
        ComponentAccessed<AllComponentTypes>...,
        ComponentUpdated<AllComponentTypes>...,
        ComponentRemoved<AllComponentTypes>...
    >;

    // Eventbus instance using the expanded types
    Eventbus<EntityAdded, 
			 EntityUpdated<AllComponentTypes>..., 
			 EntityRemoved, 
			 ComponentAdded<AllComponentTypes>..., 
			 ComponentAccessed<AllComponentTypes>..., 
			 ComponentUpdated<AllComponentTypes>..., 
			 ComponentRemoved<AllComponentTypes>...
	> eventbus_;
	// Eventbus<ALL_EVENT_TYPES...> eventbus_;

	void clearEntities()
	{
		entities_.clear();

		std::queue<Entity> empty;
		std::swap(availableEntityIds_, empty);

		for (Entity entity = 0; entity < MAX_ENTITIES; entity++) 
		{
			availableEntityIds_.push(entity);
		}
	}
};

}  // namespace Easys