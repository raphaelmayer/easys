#include <easys/ecs.hpp>
#include <easys/entity.hpp>
#include <iostream>

// This is a very basic example to demonstrate creating an entity,
// adding a component to it, and retrieving the component.

// Define a Position component structure.
// Components are simple structs or classes that contain data.
struct Position {
	float x, y;
};

int main()
{
	using namespace Easys;

	// Create an instance of the ECS class to manage our entities and components.
	ECS<Position> ecs;

	// Create a new entity. An entity is just a number.
	Entity entity = ecs.addEntity();

	// Add a Position component to the entity with initial values.
	ecs.addComponent<Position>(entity, {10.0f, 20.0f});

	// Update the component.
	auto& component = ecs.getComponent<Position>(entity);
	component.x = 0.0f;

	// Retrieve the Position component from the entity to read or modify it.
	Position& pos = ecs.getComponent<Position>(entity);
	std::cout << "Position: " << pos.x << ", " << pos.y << std::endl;

	return 0;
}