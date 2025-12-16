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

	template <typename T, typename Func>
	inline void modifyComponent(const Entity e, Func&& fn)
	{
		T& c = getComponent<T>(e);
		fn(c);
	}

	template <typename T>
	inline void modifyComponent(const Entity e, T c)
	{
		getComponent<T>(e) = c;
	}

	template <typename ComponentType>
	inline void removeComponent(const Entity entity)
	{
		auto& componentSet = getComponentSet<ComponentType>();
		componentSet.remove(entity);
	}

	template <typename... ComponentTypes>
	inline void removeComponents(const Entity entity)
	{
		(removeComponent<ComponentTypes>(entity), ...);
	}

	inline void removeComponents(const Entity entity)
	{
		removeComponents<AllComponentTypes...>(entity);
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

	template <typename... ComponentTypes>
	inline std::vector<Entity> getEntities() const
	{
		std::vector<Entity> entities;
		bool isFirstComponentType = true;

		// Iterate over each component type and intersect entities
		forEachComponentType<ComponentTypes...>(
		    [this, &entities, &isFirstComponentType]<typename T>()
		    {
			    if (isFirstComponentType)
			    {
				    entities = getComponentSet<T>().getKeys();
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

	template <typename... ComponentTypes>
	inline size_t size() const
	{
		return (... + getComponentSet<ComponentTypes>().size());
	}

	inline size_t size() const
	{
		return size<AllComponentTypes...>();
	}

	template <typename... ComponentTypes>
	inline void clear()
	{
		(getComponentSet<ComponentTypes>().clear(), ...);
	}

	inline void clear()
	{
		clear<AllComponentTypes...>();
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