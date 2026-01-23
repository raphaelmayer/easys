#pragma once

#include <algorithm>
#include <vector>

#include "entity.hpp"

namespace Easys {

// TODO: We could store a ref to ecs for convenient view member functions, but im not sure this is the right approach.

// template <typename ECSType, typename... ComponentTypes>

/**
 * @brief A lightweight, read-only snapshot of a set of entities.
 *
 * The View class owns a vector of entity IDs. It is typically used as the result of
 * Registry::getEntities() or Registry::getEntities<Components...>().
 *
 * Views are movable and copyable, and provide iterators, size queries, and utility
 * functions such as find() and contains().
 *
 * Example usage:
 * @code
 * View view = ecs.getEntities<Transform, Velocity>();
 * if(view.contains(playerEntity)) { ... }
 * for(const Entity &e : view) { ... }
 * @endcode
 */
class View {
   private:
	// ECSType& ecs;
	std::vector<Easys::Entity> entities;

   public:
	/**
	 * @brief Constructs a View by taking ownership of a vector of entities.
	 * @param ents The vector of entities to move into the view.
	 *
	 * This constructor is move-friendly: passing an rvalue vector avoids copying.
	 */
	explicit View(std::vector<Easys::Entity> ents) noexcept : entities(std::move(ents)) {}

	// explicit View(ECSType& ecs) noexcept : ecs(ecs), entities(ecs.getEntities<ComponentTypes...>()) {}

	View(View&&) noexcept = default;
	View& operator=(View&&) noexcept = default;

	View(const View&) = default;
	View& operator=(const View&) = default;

	auto begin() noexcept { return entities.begin(); }
	auto end() noexcept { return entities.end(); }

	auto begin() const noexcept { return entities.begin(); }
	auto end() const noexcept { return entities.end(); }

	/**
	 * @brief Access the internal vector of entities.
	 * @return const reference to the underlying vector
	 */
	const auto& data() const noexcept { return entities; }

	/**
	 * @brief Number of entities in the view.
	 * @return The number of entities
	 */
	size_t size() const noexcept { return entities.size(); }

	/**
	 * @brief Checks if the view is empty.
	 * @return true if the view contains no entities, false otherwise
	 */
	bool empty() const noexcept { return entities.empty(); }

	/**
	 * @brief Finds an entity in the view.
	 * @param e The entity to search for
	 * @return Iterator to the entity if found, or end() if not found
	 */
	auto find(Entity e) const noexcept { return std::find(entities.begin(), entities.end(), e); }

	/**
	 * @brief Checks if the view contains the given entity.
	 * @param e The entity to search for
	 * @return true if the entity is in the view, false otherwise
	 */
	bool contains(Entity e) const noexcept { return find(e) != entities.end(); }
};

}  // namespace Easys
