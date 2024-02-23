#pragma once

#include "entity.hpp"
#include "sparse_set.hpp"
#include <any>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <vector>

class ComponentTypeNotFoundException : public std::exception {
  public:
	ComponentTypeNotFoundException(const std::string &key) : msg_("ComponentTypeNotFoundException: " + key + " not found") {}

	const char *what() const noexcept override { return msg_.c_str(); }

  private:
	std::string msg_;
};

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
		for (auto &[type, componentSet] : componentSets)
			componentSet.remove(entity);
	}

	template <typename ComponentType>
	ComponentType &getComponent(const Entity entity)
	{
		auto &componentSet = getComponentSet<ComponentType>();
		return *std::any_cast<ComponentType>(&componentSet.get(entity));
		// could be optimized by omitting any checks and accessing elements directly, i.e
		// return *std::any_cast<ComponentType>(&componentSet[entity]);
	}

	template <typename ComponentType>
	bool hasComponent(const Entity entity) const
	{
		const auto it = componentSets.find(typeid(ComponentType));
		return it != componentSets.end() && it->second.contains(entity);
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
		for (auto &[type, componentSet] : componentSets)
			totalSize += componentSet.size();

		return totalSize;
	}

	template <typename... ComponentTypes>
	size_t size() const
	{
		size_t totalSize = 0;
		forEachComponentType<ComponentTypes...>([this, &totalSize](auto dummy) {
			using T = decltype(dummy);
			if (hasComponentType<T>()) // we ignore unknown component types. should we throw an exception?
				totalSize += getComponentSet<T>().size();
		});

		return totalSize;
	}

	void clear() { componentSets.clear(); }

	template <typename... ComponentTypes>
	void clear()
	{
		forEachComponentType<ComponentTypes...>([this](auto dummy) {
			using T = decltype(dummy);
			if (hasComponentType<T>()) // we ignore unknown component types. should we throw an exception?
				componentSets.erase(typeid(T));
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
		// could be optimized by omitting any checks and accessing elements directly, i.e componentSets[typeid(ComponentType)]
		auto it = componentSets.find(typeid(ComponentType));
		if (it == componentSets.end())
			throw ComponentTypeNotFoundException(typeid(ComponentType).name());
		return it->second;
	}

	template <typename ComponentType>
	const SparseSet<Entity, std::any> &getComponentSet() const
	{
		// could be optimized by omitting any checks and accessing elements directly
		auto it = componentSets.find(typeid(ComponentType));
		if (it == componentSets.end())
			throw ComponentTypeNotFoundException(typeid(ComponentType).name());
		return it->second;
	}

	// Check if a ComponentType has been initialized
	template <typename ComponentType>
	bool hasComponentType() const
	{
		return componentSets.find(typeid(ComponentType)) != componentSets.end();
	}
};
