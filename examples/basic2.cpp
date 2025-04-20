#include <easys/ecs.hpp>
#include <easys/entity.hpp>
#include <iostream>

// This extended example demonstrates creating multiple entities with various components,
// accessing these components, and utilizing advanced querying capabilities to manage
// entities within the ECS framework.

// Define component structures
struct Position {
	float x, y;
};

struct Velocity {
	float dx, dy;
};

int main()
{
	using namespace Easys;

	ECS ecs;

	// Creating multiple entities and adding components
	Entity firstEntity = ecs.addEntity();
	Entity secondEntity = ecs.addEntity();
	ecs.addComponent(firstEntity, Position{10.0f, 20.0f});
	ecs.addComponent(firstEntity, Velocity{1.0f, 1.0f});
	ecs.addComponent(secondEntity, Position{30.0f, 40.0f});
	// secondEntity does not have a Velocity component

	// Querying components from entities
	Position &firstPos = ecs.getComponent<Position>(firstEntity);
	Velocity &firstVel = ecs.getComponent<Velocity>(firstEntity);
	std::cout << "First Entity Position: " << firstPos.x << ", " << firstPos.y << std::endl;
	std::cout << "First Entity Velocity: " << firstVel.dx << ", " << firstVel.dy << std::endl;

	// Querying all entities
	for (auto entity : ecs.getEntities()) {
		if (ecs.hasComponent<Position>(entity)) {
			Position &pos = ecs.getComponent<Position>(entity);
			std::cout << "Entity " << entity << " Position: " << pos.x << ", " << pos.y << std::endl;
		}
	}

	// Querying entities that have a single component
	for (auto entity : ecs.getEntitiesByComponent<Position>()) {
		Position &pos = ecs.getComponent<Position>(entity);
		std::cout << "Entity " << entity << " Position: " << pos.x << ", " << pos.y << std::endl;
	}

	// Querying entities that have multiple components
	for (auto entity : ecs.getEntitiesByComponents<Position, Velocity>()) {
		Position &pos = ecs.getComponent<Position>(entity);
		Velocity &vel = ecs.getComponent<Velocity>(entity);
		std::cout << "Entity " << entity << " Position: " << pos.x << ", " << pos.y << " Velocity: " << vel.dx << ", "
		          << vel.dy << std::endl;
	}

	// Attempting to access a component not present on an entity
	try {
		Velocity &secondVel = ecs.getComponent<Velocity>(secondEntity); // This will throw an exception
	} catch (const KeyNotFoundException) {
		std::cout << "Second Entity does not have a Velocity component." << std::endl;
	}

	return 0;
}
