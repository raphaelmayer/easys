#include <easys/ecs.hpp>
#include <easys/entity.hpp>

// Example components
struct Position {
	float x, y;
	Position(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

struct Velocity {
	float vx, vy;
	Velocity(float vx = 0.0f, float vy = 0.0f) : vx(vx), vy(vy) {}
};

using ECS = Easys::ECS<Position, Velocity>;

// Example system which updates the player position based on the player velocity.
struct System {
	void update(ECS &ecs)
	{
		for (Easys::Entity e : ecs.getEntitiesByComponents<Position, Velocity>())
		{
			Position &pos = ecs.getComponent<Position>(e);
			const Velocity &vel = ecs.getComponent<Velocity>(e);

			pos.x += vel.vx;
			pos.y += vel.vy;
		}
	}
};

// Main game loop
int main()
{
	ECS ecs;
	System system;

	// Create an entity and add components
	Easys::Entity player = ecs.addEntity();
	ecs.addComponent(player, Position(1.0f, 1.0f));
	ecs.addComponent(player, Velocity(0.1f, 0.0f));

	bool isRunning = true;
	while (isRunning)
	{
		// handle input (for example in another system)

		system.update(ecs);  // handle system update

		// handle rendering (for example in another system)
		Position p = ecs.getComponent<Position>(player);
		std::cout << "x: " << p.x << ", y: " << p.y << std::endl;
	}

	return 0;
}
