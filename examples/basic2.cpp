#include <easys.hpp>
#include <iostream>

// Define component structures
struct Position {
	float x, y;
};

struct Velocity {
	float dx, dy;
};

int main()
{
	ECS ecs;

	// Creating multiple entities and adding components
	Entity firstEntity = ecs.addEntity();
	ecs.addComponent(firstEntity, Position{10.0f, 20.0f});
	ecs.addComponent(firstEntity, Velocity{1.0f, 1.0f});

	Entity secondEntity = ecs.addEntity();
	ecs.addComponent(secondEntity, Position{30.0f, 40.0f});
	// This entity does not have a Velocity component

	// Querying components from entities
	Position &firstPos = ecs.getComponent<Position>(firstEntity);
	Velocity &firstVel = ecs.getComponent<Velocity>(firstEntity);
	std::cout << "First Entity Position: " << firstPos.x << ", " << firstPos.y << std::endl;
	std::cout << "First Entity Velocity: " << firstVel.dx << ", " << firstVel.dy << std::endl;

	// Handling entities with multiple different components
	// Example: Querying entities that have both Position and Velocity components
	for (auto entity : ecs.getEntitiesByComponents<Position, Velocity>()) {
		Position &pos = ecs.getComponent<Position>(entity);
		Velocity &vel = ecs.getComponent<Velocity>(entity);
		std::cout << "Entity " << entity << " Position: " << pos.x << ", " << pos.y << " Velocity: " << vel.dx << ", "
		          << vel.dy << std::endl;
	}

	// Example: Attempting to access a component not present on an entity
	try {
		Velocity &secondVel = ecs.getComponent<Velocity>(secondEntity); // This will throw an exception
	} catch (const std::exception &e) {
		std::cout << "Second Entity does not have a Velocity component." << std::endl;
	}

	// This extended example demonstrates creating multiple entities with various components,
	// accessing these components, and utilizing advanced querying capabilities to manage
	// entities within the ECS framework provided by EasyS.

	return 0;
}
