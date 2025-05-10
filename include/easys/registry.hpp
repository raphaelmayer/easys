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
	void addComponent(const Entity entity, const ComponentType& component)
	{
		auto& componentSet = getComponentSet<ComponentType>();
		componentSet.set(entity, std::move(component));
	}

	template <typename ComponentType>
	void removeComponent(const Entity entity)
	{
		auto& componentSet = getComponentSet<ComponentType>();
		componentSet.remove(entity);
	}

	void removeComponents(const Entity entity)
	{
		forEachComponentType<AllComponentTypes...>(
		    [&]<typename Component>()
		    {
			    removeComponent<Component>(entity);
		    });
	}

	template <typename... ComponentTypes>
	void removeComponents(const Entity entity)
	{
		forEachComponentType<ComponentTypes...>(
		    [&]<typename Component>()
		    {
			    removeComponent<Component>(entity);
		    });
	}

	template <typename ComponentType>
	ComponentType& getComponent(const Entity entity)
	{
		auto& componentSet = getComponentSet<ComponentType>();
		// could be optimized with direct access. get() calls contains() internally
		return componentSet.get(entity);
	}

	template <typename ComponentType>
	const ComponentType& getComponent(const Entity entity) const
	{
		const auto& componentSet = getComponentSet<ComponentType>();
		// could be optimized with direct access. get() calls contains() internally
		return componentSet.get(entity);
	}

	template <typename ComponentType>
	bool hasComponent(const Entity entity) const
	{
		return getComponentSet<ComponentType>().contains(entity);
	}

	template <typename ComponentType>
	const std::vector<Entity>& getEntitiesByComponent() const
	{
		return getComponentSet<ComponentType>().getKeys();
	}

	template <typename... ComponentTypes>
	std::vector<Entity> getEntitiesByComponents() const
	{
		std::vector<Entity> entities;
		bool isFirstComponentType = true;

		// Helper lambda to intersect two sorted vectors
		auto intersect = [](const std::vector<Entity>& v1, const std::vector<Entity>& v2)
		{
			std::vector<Entity> v_intersection;
			std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(v_intersection));
			return v_intersection;
		};

		// Iterate over each component type and intersect entities
		forEachComponentType<ComponentTypes...>(
		    [this, &entities, &isFirstComponentType, &intersect]<typename T>()
		    {
			    // We sort here. This is not optimal. We probably want to lazily
			    // sort based on a flag (refer to github issue #7):
			    auto componentEntities = getEntitiesByComponent<T>();
			    std::sort(componentEntities.begin(), componentEntities.end());
			    // Temporary fix end
			    if (isFirstComponentType)
			    {
				    entities = componentEntities;
				    isFirstComponentType = false;
			    } else
			    {
				    entities = intersect(entities, componentEntities);
			    }
		    });

		return entities;
	}

	size_t size() const
	{
		size_t totalSize = 0;

		forEachComponentType<AllComponentTypes...>(
		    [this, &totalSize]<typename T>()
		    {
			    totalSize += getComponentSet<T>().size();
		    });

		return totalSize;
	}

	template <typename... ComponentTypes>
	size_t size() const
	{
		size_t totalSize = 0;

		forEachComponentType<ComponentTypes...>(
		    [this, &totalSize]<typename T>()
		    {
			    totalSize += getComponentSet<T>().size();
		    });

		return totalSize;
	}

	void clear()
	{
		forEachComponentType<AllComponentTypes...>(
		    [this]<typename T>()
		    {
			    getComponentSet<T>().clear();
		    });
	}

	template <typename... ComponentTypes>
	void clear()
	{
		forEachComponentType<ComponentTypes...>(
		    [this]<typename T>()
		    {
			    getComponentSet<T>().clear();
		    });
	}

   private:
	template <typename T>
	static constexpr bool isRegisteredComponent = (std::is_same_v<T, AllComponentTypes> || ...);

	template <typename... ComponentTypes, typename Func>
	void forEachComponentType(Func&& f) const
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
	SparseSet<Entity, ComponentType>& getComponentSet()
	{
		static_assert(isRegisteredComponent<ComponentType>, "Tried to access an unregistered component type.");
		return std::get<SparseSet<Entity, ComponentType>>(componentSets);
	}

	template <typename ComponentType>
	const SparseSet<Entity, ComponentType>& getComponentSet() const
	{
		static_assert(isRegisteredComponent<ComponentType>, "Tried to access an unregistered component type.");
		return std::get<SparseSet<Entity, ComponentType>>(componentSets);
	}
};

}  // namespace Easys