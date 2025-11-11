#pragma once

#include <any>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "entity.hpp"
#include "sparse_set.hpp"

namespace Easys {

template <typename... AllComponentTypes>
class Registry {
   private:
	mutable std::tuple<SparseSet<Entity, AllComponentTypes>...> componentSets;

   public:
	template <typename ComponentType>
	inline void addComponent(const Entity entity, const ComponentType& component)
	{
		auto& componentSet = getComponentSet<ComponentType>();
		componentSet.set(entity, std::move(component));
	}

	template <typename ComponentType>
	inline void removeComponent(const Entity entity)
	{
		auto& componentSet = getComponentSet<ComponentType>();
		componentSet.remove(entity);
	}

	// unused now (if we actually delegate everything to ECS::removeComponent<T>(const Entity&)
	inline void removeComponents(const Entity entity)
	{
		// forEachComponentType<AllComponentTypes...>(
		//     [&]<typename Component>()
		//     {
		//	    removeComponent<Component>(entity);
		//     });
		(removeComponent<AllComponentTypes>(entity), ...);
		//(removeComponents<AllComponentTypes...>(entity);
	}

	// unused now (if we actually delegate everything to ECS::removeComponent<T>(const Entity&)
	template <typename... ComponentTypes>
	inline void removeComponents(const Entity entity)
	{
		// forEachComponentType<ComponentTypes...>(
		//     [&]<typename Component>()
		//     {
		//	    removeComponent<Component>(entity);
		//     });
		(removeComponent<ComponentTypes>(entity), ...);
		// could also just access remove directly and remove the single component version.
		// It is not necessary, as the removeCOmponents<Ts...>() can handle all cases.
		// But we would miss out on ECS>>removeComponent<T>(), which is where we plan to dispatch events.
		// Would we though? We can still call it with a single type and implement ECS::removeComponent<T>() by using
		// Registry>>removeComponents<T>() internally.
		(getComponentSet<ComponentTypes>().remove(entity), ...);
	}

	template <typename ComponentType>
	inline ComponentType& getComponent(const Entity entity)
	{
		auto& componentSet = getComponentSet<ComponentType>();
		// could be optimized with direct access. get() calls contains() internally
		return componentSet.get(entity);
	}

	template <typename ComponentType>
	inline const ComponentType& getComponent(const Entity entity) const
	{
		const auto& componentSet = getComponentSet<ComponentType>();
		// could be optimized with direct access. get() calls contains() internally
		return componentSet.get(entity);
	}

	template <typename ComponentType>
	inline bool hasComponent(const Entity entity) const
	{
		return getComponentSet<ComponentType>().contains(entity);
	}

	template <typename ComponentType>
	inline const std::vector<Entity>& getEntitiesByComponent() const
	{
		return getComponentSet<ComponentType>().getKeys();
	}

	template <typename... ComponentTypes>
	inline std::vector<Entity> getEntitiesByComponents() const
	{
		std::vector<Entity> entities;
		bool isFirstComponentType = true;

		// Iterate over each component type and intersect entities
		forEachComponentType<ComponentTypes...>(
		    [this, &entities, &isFirstComponentType]<typename T>()
		    {
			    if (isFirstComponentType)
			    {
				    entities = getEntitiesByComponent<T>();
				    isFirstComponentType = false;
			    }

			    else
			    {
				    std::vector<Easys::Entity> newEntities;
				    newEntities.reserve(entities.size());

				    for (const auto& e : entities)
				    {
					    if (hasComponent<T>(e))
					    {
						    newEntities.push_back(e);
					    }
				    }

				    entities = std::move(newEntities);
			    }
		    });

		return entities;
	}

	inline size_t size() const
	{
		// size_t totalSize = 0;

		// forEachComponentType<AllComponentTypes...>(
		//     [this, &totalSize]<typename T>()
		//     {
		// 	    totalSize += getComponentSet<T>().size();
		//     });

		// return totalSize;
		return size<AllComponentTypes...>();
	}

	template <typename... ComponentTypes>
	inline size_t size() const
	{
		// size_t totalSize = 0;

		// 1)
		// forEachComponentType<ComponentTypes...>(
		//     [this, &totalSize]<typename T>()
		//     {
		// 	    totalSize += getComponentSet<T>().size();
		//     });
		// auto sizeHelper = [&]<typename T>{
		// 	totalSize += getComponentSet<T>().size();
		// };

		// 2)
		// (sizeHelper.template operator()<AllComponentTypes>(), ...);

		// return totalSize;

		// 3
		return (... + getComponentSet<ComponentTypes>().size());
	}

	inline void clear()
	{
		forEachComponentType<AllComponentTypes...>(
		    [this]<typename T>()
		    {
			    getComponentSet<T>().clear();
		    });
		// clear<AllComponentTypes>();
	}

	template <typename... ComponentTypes>
	inline void clear()
	{
		forEachComponentType<ComponentTypes...>(
		    [this]<typename T>()
		    {
			    getComponentSet<T>().clear();
		    });
		// (getComponentSet<ComponentTypes>().clear(), ...)
	}

   private:
	template <typename T>
	static constexpr bool isRegisteredComponent = (std::is_same_v<T, AllComponentTypes> || ...);

	template <typename... ComponentTypes, typename Func>
	inline void forEachComponentType(Func&& f) const
	{
		// MSVC does not support multiple expressions in a fold statement so we use this small helper
		auto staticAssertAndCall = [&f]<typename T>()
		{
			static_assert(isRegisteredComponent<T>, "Tried to access an unregistered component type in ECS.");
			f.template operator()<T>();
		};

		(staticAssertAndCall.template operator()<ComponentTypes>(), ...);
	}

	template <typename ComponentType>
	inline SparseSet<Entity, ComponentType>& getComponentSet()
	{
		static_assert(isRegisteredComponent<ComponentType>, "Tried to access an unregistered component type.");
		return std::get<SparseSet<Entity, ComponentType>>(componentSets);
	}

	template <typename ComponentType>
	inline const SparseSet<Entity, ComponentType>& getComponentSet() const
	{
		static_assert(isRegisteredComponent<ComponentType>, "Tried to access an unregistered component type.");
		return std::get<SparseSet<Entity, ComponentType>>(componentSets);
	}
};

}  // namespace Easys