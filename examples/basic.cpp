#include <ecs/ecs.hpp>
#include <ecs/entity.hpp>
#include <iostream>

struct Position {
	int x, y;
};

int main(int argc, char *argv[])
{
	ECS ecs;

	// create and add entity with component
	Entity entity = ecs.addEntity();
	Position component{0, 0};
	ecs.addComponent<Position>(entity, component);

	// update component
	Position &position = ecs.getComponent<Position>(entity);
	position.x = 1;
	position.y = 2;

	// component updated
	Position newPosition = ecs.getComponent<Position>(entity);
	std::cout << "x: " << newPosition.x << ", y: " << newPosition.y << std::endl;

	return 0;
}