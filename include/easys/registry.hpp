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
	std::unordered_map<std::type_index, SparseSet<Entity, std::any>> componentSets;

  public:
	template <typename ComponentType>
	void addComponent(const Entity entity, const ComponentType component)
	{
		auto [it, inserted] = componentSets.try_emplace(typeid(ComponentType));
		if (inserted) {
			it->second = SparseSet<Entity, std::any>();
		}
		it->second.set(entity, std::any(component));
	}

	template <typename ComponentType>
	void removeComponent(const Entity entity)
	{
		auto &componentSet = getComponentSet<ComponentType>();
		componentSet.remove(entity);
	}

	void removeComponents(const Entity entity)
	{
		for (auto &[type, components] : componentSets) {
			components.remove(entity);
		}
	}

	template <typename ComponentType>
	ComponentType &getComponent(const Entity entity)
	{
		auto &componentSet = getComponentSet<ComponentType>();
		std::any &componentAny = componentSet.get(entity);
		return *std::any_cast<ComponentType>(&componentAny);
	}

	template <typename ComponentType>
	bool hasComponent(const Entity entity) const
	{
		try {
			const auto &componentSet = getComponentSet<ComponentType>();
			return componentSet.contains(entity);
		} catch (const std::exception &) {
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

	size_t size() const
	{
		size_t totalSize = 0;
		for (auto &[type, components] : componentSets)
			totalSize += components.size();

		return totalSize;
	}

	template <typename... ComponentTypes>
	size_t size() const
	{
		size_t totalSize = 0;
		forEachComponentType<ComponentTypes...>([this, &totalSize](auto dummy) {
			using T = decltype(dummy);
			try {
				totalSize += getComponentSet<T>().size();
			} catch (std::out_of_range) {
				// should this throw, if component types contains unknown component type?
				// currently we just ignore it and sum all known component types
			}
		});

		return totalSize;
	}

	void clear()
	{
		for (auto &[type, components] : componentSets)
			components.clear();
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

  private:
	template <typename ComponentType>
	SparseSet<Entity, std::any> &getComponentSet()
	{
		return componentSets.at(typeid(ComponentType));
	}

	template <typename ComponentType>
	const SparseSet<Entity, std::any> &getComponentSet() const
	{
		return componentSets.at(typeid(ComponentType));
	}
};
