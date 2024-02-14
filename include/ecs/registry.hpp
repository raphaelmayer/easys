#pragma once

#include "entity.hpp"
#include "sparse_set.hpp"
#include <any>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>

struct Registry {
	std::unordered_map<std::type_index, std::any> componentSets;

	template <typename ComponentType>
	void addComponent(Entity entity, ComponentType component)
	{
		auto [it, inserted] = componentSets.try_emplace(typeid(ComponentType), std::any());
		if (inserted) {
			it->second = std::make_any<SparseSet<Entity, ComponentType>>();
		}
		std::any_cast<SparseSet<Entity, ComponentType> &>(it->second).add(entity, component);
	}

	template <typename ComponentType>
	void removeComponent(Entity entity)
	{
		auto &componentSet = getComponentSet<ComponentType>();
		componentSet.remove(entity);
	}

	void removeComponents(Entity entity)
	{
		// TODO: does not work with current implementation
	}

	template <typename ComponentType>
	ComponentType &getComponent(Entity entity)
	{
		auto &componentSet = getComponentSet<ComponentType>();
		return componentSet.get(entity);
	}

	template <typename ComponentType>
	bool hasComponent(Entity entity) const
	{
		try {
			const auto &componentSet = getComponentSet<ComponentType>();
			return componentSet.contains(entity);
		} catch (const std::exception) {
			return false;
		}
	}

	template <typename ComponentType>
	const std::vector<Entity> &getEntitiesByType() const
	{
		return getComponentSet<ComponentType>().getKeys();
	}

	template <typename ComponentType>
	std::vector<ComponentType> &getComponentsByType()
	{
		return getComponentSet<ComponentType>().getValues();
	}

	template <typename... ComponentTypes, typename Func>
	void forEachComponentType(Func f) const
	{
		(f(ComponentTypes{}), ...);
	}

	template <typename... ComponentTypes>
	size_t size() const
	{
		size_t totalSize = 0;
		forEachComponentType<ComponentTypes...>([this, &totalSize](auto dummy) {
			using T = decltype(dummy);
			try {
				totalSize += getComponentSet<T>().size(); // Explicitly call the single-type version of `size`
			} catch (std::out_of_range) {
				// should this throw, if component types contains unknown component type?
				// currently we just ignore it and sum all known component types
			}
		});

		return totalSize;
	}

  private:
	template <typename ComponentType>
	SparseSet<Entity, ComponentType> &getComponentSet()
	{
		// return std::any_cast<SparseSet<Entity, ComponentType>&>(componentSets[typeid(ComponentType)]);
		return std::any_cast<SparseSet<Entity, ComponentType> &>(componentSets.at(typeid(ComponentType)));
	}

	template <typename ComponentType>
	const SparseSet<Entity, ComponentType> &getComponentSet() const
	{
		return std::any_cast<const SparseSet<Entity, ComponentType> &>(componentSets.at(typeid(ComponentType)));
	}
};
