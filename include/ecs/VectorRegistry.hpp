#pragma once

#include <any>
#include <functional>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include "Entity.hpp"

// This is the core of our ECS. It stores an std::unordered_map for every ComponentType.
// New components can be added without needing to adjust anything. They will be handled automatically.
// The disadvantage is a hit in performance, since we now leverage type erasure. Components have to be cast
// to their respective ComponentType each time they are retrieved from the registry.
struct Registry {
	std::unordered_map<std::type_index, std::vector<std::any>> components;

	template <typename T>
	void addComponent(const Entity entity, const T component)
	{
		auto &compVector = components[typeid(T)];
		if (compVector.size() <= entity) {
			compVector.resize((entity + 100));
		}
		compVector[entity] = component;
	}

	template <typename T>
	void removeComponent(const Entity entity)
	{
		auto &compVector = components[typeid(T)];
		if (entity < compVector.size()) {
			compVector[entity] = std::any(); // Resetting the std::any object at index 'e'
		}
	}

	void removeComponents(const Entity e)
	{
		for (auto &[type, compVector] : components) {
			if (e < compVector.size()) {
				compVector[e] = std::any(); // Resetting the std::any object at index 'e'
			}
		}
	}

	// Method is not const, because components[typeid(T)] "could" create a new key, if not already present.
	// In practice this should not happen, because we know all components at compile time and set up the ecs
	// accordingly.
	template <typename T>
	T &getComponent(const Entity entity)
	{
		auto &compVector = components[typeid(T)];
		if (entity < compVector.size()) {
			return std::any_cast<T &>(compVector[entity]);
		}
		throw std::runtime_error("Component not found");
	}

	template <typename T>
	std::vector<std::reference_wrapper<T>> getAllComponentsByType() // not const, because: see registry.getComponent
	{
		std::vector<std::reference_wrapper<T>> allComponentsOfType;
		for (std::any &anyComponent : components[typeid(T)]) {
			if (anyComponent.has_value()) {
				allComponentsOfType.push_back(std::any_cast<T &>(anyComponent));
			}
		}
		return allComponentsOfType;
	}

	template <typename T>
	bool hasComponent(const Entity entity) const
	{
		if (!components.contains(typeid(T))) {
			return false;
		}
		const auto &compVector = components.at(typeid(T));
		return entity < compVector.size() && compVector[entity].has_value();
	}

	size_t size() const
	{
		size_t total = 0;
		for (const auto &pair : components) {
			const auto &compVector = pair.second;
			for (const auto &comp : compVector) {
				if (comp.has_value()) {
					++total;
				}
			}
		}
		return total;
	}
};
