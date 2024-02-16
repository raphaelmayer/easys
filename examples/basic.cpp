#include <easys.hpp>
#include <iostream>

// Define a Position component structure.
// Components are simple structs or classes that contain data.
struct Position {
	float x, y;
};

int main()
{
	// Create an instance of the ECS class to manage our entities and components.
	ECS ecs;

	// Create a new entity. This returns an Entity identifier.
	Entity myEntity = ecs.addEntity();

	// Add a Position component to the entity with initial values.
	ecs.addComponent(myEntity, Position{10.0f, 20.0f});

	// Retrieve the Position component from the entity to read or modify it.
	Position &pos = ecs.getComponent<Position>(myEntity);
	std::cout << "Position: " << pos.x << ", " << pos.y << std::endl;

	// This is a basic example to demonstrate creating an entity,
	// adding a component to it, and retrieving the component.

	return 0;
}