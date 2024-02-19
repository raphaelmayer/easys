#pragma once

#include "entity.hpp"
#include "sparse_set.hpp"
#include <any>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <vector>

class Registry {
  private:
	std::unordered_map<std::type_index, std::any> componentSets;

  public:
	template <typename ComponentType>
	void addComponent(Entity entity, ComponentType component)
	{
		auto [it, inserted] = componentSets.try_emplace(typeid(ComponentType), std::any());
		if (inserted) {
			it->second = std::make_any<SparseSet<Entity, ComponentType>>();
		}
		std::any_cast<SparseSet<Entity, ComponentType> &>(it->second).set(entity, component);
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
	const std::vector<Entity> &getEntitiesByComponent() const
	{
		return getComponentSet<ComponentType>().getKeys();
	}

	template <typename... ComponentTypes>
	std::vector<Entity> getEntitiesByComponents() const
	{
		std::vector<Entity> entities;
		bool isFirstComponentType = true;

		// Helper lambda to intersect two sorted vectors
		auto intersect = [](const std::vector<Entity> &v1, const std::vector<Entity> &v2) {
			std::vector<Entity> v_intersection;
			std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(v_intersection));
			return v_intersection;
		};

		// Iterate over each component type and intersect entities
		forEachComponentType<ComponentTypes...>([this, &entities, &isFirstComponentType, &intersect](auto dummy) {
			using T = decltype(dummy);
			const auto &componentEntities = getEntitiesByComponent<T>();
			if (isFirstComponentType) {
				entities = componentEntities;
				isFirstComponentType = false;
			} else {
				entities = intersect(entities, componentEntities);
			}
		});

		return entities;
	}

	template <typename ComponentType>
	std::vector<ComponentType> &getComponentsByType()
	{
		return getComponentSet<ComponentType>().getValues();
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

	template <typename... ComponentTypes>
	void clear()
	{
		forEachComponentType<ComponentTypes...>([this](auto dummy) {
			using T = decltype(dummy);
			getComponentSet<T>().clear();
		});
	}

  private:
	template <typename... ComponentTypes, typename Func>
	void forEachComponentType(Func f) const
	{
		// static assert as a fallback
		static_assert(sizeof...(ComponentTypes) > 0, "You must specify at least one component type.");
		(f(ComponentTypes{}), ...);
	}

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
