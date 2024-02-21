#define CATCH_CONFIG_RUNNER

#include <catch2/catch.hpp>
#include <chrono>
#include <easys/ecs.hpp>
#include <easys/entity.hpp>

#define NUM_ENT MAX_ENTITIES // number of entities
#define NUM_COM 1            // number of components per entity

struct System {
	virtual void update(ECS &ecs, double deltaTime) = 0;
};

struct Position {
	float x, y;
};

struct RigidBody {
	float vx, vy;
};

struct Data {
	std::string data;
};

struct Health {
	int health;
	int maxHealth;
};

struct Damage {
	int damage;
};

// CATCH_CONFIG_RUNNER tells catch2, that we will implement our own main function to config the test runner.
int main(int argc, char *argv[])
{
	Catch::Session session;

	// Set the configuration to show all test results, including successful tests
	session.configData().showSuccessfulTests = true;

	// Set the reporter to 'console'
	session.configData().reporterName = "compact";

	// You can add additional configuration here if needed

	// Run the Catch2 session
	return session.run(argc, argv);
}

std::string formatEntCompInfo(const std::string functionName, const int numEntities, const int numComponents)
{
	std::ostringstream oss;
	oss << functionName << "(): e: " << numEntities << ", c: " << numComponents * numEntities
	    << ", c/e: " << numComponents;
	return oss.str();
}

// This function is a helper to run benchmarks. It  that takes a lambda function as an argument.
// This lambda function will contain the code to benchmark.
template <typename Func>
void benchmarkSection(Func func, const std::string &sectionName)
{
	auto start = std::chrono::high_resolution_clock::now();

	func(); // Execute the lambda function

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = end - start;

	// CAPTURE(sectionName, elapsed.count());
	SUCCEED("Benchmark completed for " + sectionName + ": " + std::to_string(elapsed.count()) + " ms");
}

TEST_CASE("ECS Benchmark", "[ECS]")
{
	struct TestComponent {
		int value;
	};

	struct AnotherComponent {
		float value;
	};

	class MockSystem : public System {
	  public:
		bool updateCalled = false;

		void update(ECS &ecs, double deltaTime) override { updateCalled = true; }
	};

	SECTION("Benchmarking Entity Addition")
	{
		ECS ecs;

		benchmarkSection(
		    [&] {
			    for (int i = 0; i < NUM_ENT; i++) {
				    ecs.addEntity();
			    }
		    },
		    formatEntCompInfo("addEntity", NUM_ENT, NUM_COM));
	}

	SECTION("Benchmarking Entity Removal")
	{
		ECS ecs;

		for (int i = 0; i < NUM_ENT; i++) {
			Entity e = ecs.addEntity();
		}

		benchmarkSection(
		    [&] {
			    for (int i = 0; i < NUM_ENT; i++) {
				    ecs.removeEntity(i);
			    }
		    },
		    formatEntCompInfo("removeEntity", NUM_ENT, NUM_COM));
	}

	SECTION("Benchmarking Component Addition")
	{
		ECS ecs;
		TestComponent c = TestComponent{};

		for (int i = 0; i < NUM_ENT; i++) {
			Entity e = ecs.addEntity();
		}

		benchmarkSection(
		    [&] {
			    for (int i = 0; i < NUM_ENT; i++) {
				    ecs.addComponent<TestComponent>(i, c);
			    }
		    },
		    formatEntCompInfo("addComponent", NUM_ENT, NUM_COM));
	}

	SECTION("Benchmarking Component Addition 2")
	{
		ECS ecs;
		TestComponent c = TestComponent{};
		AnotherComponent a = AnotherComponent{};

		for (int i = 0; i < NUM_ENT; i++) {
			Entity e = ecs.addEntity();
		}

		benchmarkSection(
		    [&] {
			    for (int i = 0; i < NUM_ENT; i++) {
				    ecs.addComponent<TestComponent>(i, c);
				    ecs.addComponent<AnotherComponent>(i, a);
			    }
		    },
		    formatEntCompInfo("addComponent", NUM_ENT, NUM_COM * 2));
	}

	SECTION("Benchmarking Component Removal")
	{
		ECS ecs;
		TestComponent c = TestComponent{};

		for (int i = 0; i < NUM_ENT; i++) {
			Entity e = ecs.addEntity();
			ecs.addComponent<TestComponent>(e, c);
		}

		benchmarkSection(
		    [&] {
			    for (int i = 0; i < NUM_ENT; i++) {
				    ecs.removeComponent<TestComponent>(i);
			    }
		    },
		    formatEntCompInfo("removeComponent", NUM_ENT, NUM_COM));
	}

	SECTION("Benchmarking Component Retrieval")
	{
		ECS ecs;
		TestComponent c = TestComponent{};

		for (int i = 0; i < NUM_ENT; i++) {
			Entity e = ecs.addEntity();
			ecs.addComponent<TestComponent>(e, c);
		}

		benchmarkSection(
		    [&] {
			    for (int i = 0; i < NUM_ENT; i++) {
				    ecs.getComponent<TestComponent>(i);
			    }
		    },
		    formatEntCompInfo("getComponent", NUM_ENT, NUM_COM));
	}

	SECTION("Benchmarking Component Existence Check")
	{
		ECS ecs;
		TestComponent c = TestComponent{};

		for (int i = 0; i < NUM_ENT; i++) {
			Entity e = ecs.addEntity();
			ecs.addComponent<TestComponent>(e, c);
		}

		benchmarkSection(
		    [&] {
			    for (int i = 0; i < NUM_ENT; i++) {
				    ecs.hasComponent<TestComponent>(i);
			    }
		    },
		    formatEntCompInfo("hascomponent", NUM_ENT, NUM_COM));
	}

	SECTION("Benchmarking Component Existence Check")
	{
		ECS ecs;
		TestComponent c = TestComponent{};

		for (int i = 0; i < NUM_ENT; i++) {
			Entity e = ecs.addEntity();
		}
		ecs.addComponent<TestComponent>(0, c);

		benchmarkSection(
		    [&] {
			    for (int i = 0; i < NUM_ENT; i++) {
				    ecs.hasComponent<TestComponent>(i);
			    }
		    },
		    formatEntCompInfo("hasComponent", NUM_ENT, 0));
	}

	SECTION("Benchmarking simulation with multiple components per entity and systems 1")
	{
		ECS ecs;
		Position p = Position{};
		RigidBody r = RigidBody{};

		struct TestPhysicsSystem : public System {
			void update(ECS &ecs, double deltaTime)
			{
				for (const Entity &entity : ecs.getEntities()) {
					if (ecs.hasComponent<RigidBody>(entity) && ecs.hasComponent<Position>(entity)) {
						auto &position = ecs.getComponent<Position>(entity);
						auto &rigidBody = ecs.getComponent<RigidBody>(entity);
						position.x += rigidBody.vx;
						position.y += rigidBody.vy;
					}
				}
			}
		};

		struct TestUpdateSystem : public System {
			void update(ECS &ecs, double deltaTime)
			{
				for (const Entity &entity : ecs.getEntities()) {
					if (ecs.hasComponent<RigidBody>(entity) && ecs.hasComponent<Position>(entity)) {
						auto &position = ecs.getComponent<Position>(entity);
						auto &rigidBody = ecs.getComponent<RigidBody>(entity);
						position.x = rigidBody.vx;
						position.y = rigidBody.vy;
					}
				}
			}
		};

		TestPhysicsSystem testPhysicsSystem = TestPhysicsSystem();
		TestUpdateSystem testUpdateSystem = TestUpdateSystem();
		double deltaTime = 0.0;

		for (int i = 0; i < NUM_ENT; i++) {
			Entity e = ecs.addEntity();
			ecs.addComponent<Position>(e, p);
			ecs.addComponent<RigidBody>(e, r);
		}

		benchmarkSection(
		    [&] {
			    testPhysicsSystem.update(ecs, deltaTime);
			    testUpdateSystem.update(ecs, deltaTime);
			    testPhysicsSystem.update(ecs, deltaTime);
			    testUpdateSystem.update(ecs, deltaTime);
			    testPhysicsSystem.update(ecs, deltaTime);
			    testUpdateSystem.update(ecs, deltaTime);
		    },
		    formatEntCompInfo("2 systems, 3 updates", NUM_ENT, NUM_COM * 2));
	}

	SECTION("Benchmarking complex simulation with diverse components and systems 2")
	{
		ECS ecs;

		// Components initialization
		Position position = Position{};
		RigidBody velocity = RigidBody{};
		Data data = Data{};
		Health health = Health{100, 100};
		Damage damage = Damage{10};

		// Systems definition
		struct MovementSystem : public System {
			void update(ECS &ecs, double deltaTime) override
			{
				for (const Entity &entity : ecs.getEntities()) {
					if (ecs.hasComponent<RigidBody>(entity) && ecs.hasComponent<Position>(entity)) {
						auto &position = ecs.getComponent<Position>(entity);
						auto &rigidBody = ecs.getComponent<RigidBody>(entity);
						position.x += rigidBody.vx * deltaTime;
						position.y += rigidBody.vy * deltaTime;
					}
				}
			}
		};

		struct DataSystem : public System {
			void update(ECS &ecs, double deltaTime) override
			{
				for (const Entity &entity : ecs.getEntities()) {
					if (ecs.hasComponent<Data>(entity)) {
						auto &data = ecs.getComponent<Data>(entity);
						// Update data with arbitrary logic
						data.data = "new data";
					}
				}
			}
		};

		struct MoreComplexSystem : public System {
			void update(ECS &ecs, double deltaTime) override
			{
				for (const Entity &entity : ecs.getEntities()) {
					if (ecs.hasComponent<Data>(entity)) {
						auto &pos = ecs.getComponent<Position>(entity);
						auto &vel = ecs.getComponent<RigidBody>(entity);
						auto &data = ecs.getComponent<Data>(entity);
						pos = {0, 0};
						vel = {1, 1};
						data.data = "data";
					}
				}
			}
		};

		struct HealthSystem : public System {
			void update(ECS &ecs, double deltaTime) override
			{
				for (const Entity &entity : ecs.getEntities()) {
					if (ecs.hasComponent<Health>(entity)) {
						auto &health = ecs.getComponent<Health>(entity);
						if (health.health > health.maxHealth)
							health.health = health.maxHealth;
						if (health.health < health.maxHealth)
							health.health = 0;
					}
				}
			}
		};

		struct DamageSystem : public System {
			void update(ECS &ecs, double deltaTime) override
			{
				for (const Entity &entity : ecs.getEntities()) {
					if (ecs.hasComponent<Health>(entity) && ecs.hasComponent<Damage>(entity)) {
						auto &health = ecs.getComponent<Health>(entity);
						auto &damage = ecs.getComponent<Damage>(entity);
						health.health -= damage.damage; // Simplified damage logic
					}
				}
			}
		};

		// Systems instantiation
		MovementSystem movementSystem = MovementSystem();
		DataSystem dataSystem = DataSystem();
		MoreComplexSystem moreComplexSystem = MoreComplexSystem();
		HealthSystem healthSystem = HealthSystem();
		DamageSystem damageSystem = DamageSystem();

		double deltaTime = 0.016; // Assuming 60 FPS for deltaTime

		for (int i = 0; i < NUM_ENT; i++) {
			Entity e = ecs.addEntity();
			ecs.addComponent<Position>(e, position);
			ecs.addComponent<RigidBody>(e, velocity);
			ecs.addComponent<Data>(e, data);
			ecs.addComponent<Health>(e, health);
			ecs.addComponent<Damage>(e, damage);
		}

		benchmarkSection(
		    [&] {
			    movementSystem.update(ecs, deltaTime);
			    dataSystem.update(ecs, deltaTime);
			    moreComplexSystem.update(ecs, deltaTime);
			    healthSystem.update(ecs, deltaTime);
			    damageSystem.update(ecs, deltaTime);
		    },
		    formatEntCompInfo("5 systems, 1 update", NUM_ENT, NUM_COM * 5));
	}
}
