#define CATCH_CONFIG_RUNNER

#include <catch2/catch.hpp>
#include <chrono>
#include <easys.hpp>

#define NUM_ENT MAX_ENTITIES // number of entities
#define NUM_COM 1            // number of components per entity

struct System {
	virtual void update(ECS &ecs, double deltaTime) = 0;
};

struct Position {
	unsigned x, y;
};

struct RigidBody {
	unsigned vx, vy;
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

	SECTION("Benchmarking simulation with multiple components per entity and systems")
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
		    formatEntCompInfo("im, 2 systems, 3 updates", NUM_ENT, NUM_COM * 2));
	}
}
