# EasyS: Manage entities and components with minimal fuss

[![Build and test](https://github.com/raphaelmayer/easys/actions/workflows/cmake-build-and-test.yml/badge.svg)](https://github.com/raphaelmayer/easys/actions/workflows/cmake-build-and-test.yml)

EasyS is a minimalist, header-only C++ library designed to streamline the development of applications using the Entity Component System (ECS) architecture. With a focus on simplicity, flexibility, and ease of use, it offers developers an unopinionated foundation to build efficient, high-performance systems without the overhead of external dependencies.

TLDR: EasyS provides the essential tools to create, manage, and iterate on entities and components with minimal fuss.


## Quick Start Guide

This section will walk you through the basics of creating entities, adding components to them, and querying these components.

```
#include <easys.hpp>
#include <iostream>

// Define a Position component structure.
// Components are simple structs or classes that contain data.
struct Position {
    float x, y;
};

int main() {
    // Create an instance of the ECS class to manage our entities and components.
    ECS ecs;

    // Create a new entity. This returns an Entity identifier.
    Entity myEntity = ecs.addEntity();

    // Add a Position component to the entity with initial values.
    ecs.addComponent(myEntity, Position{10.0f, 20.0f});

    // Retrieve the Position component from the entity to read or modify it.
    Position& pos = ecs.getComponent<Position>(myEntity);
    std::cout << "Position: " << pos.x << ", " << pos.y << std::endl;

    // This is a basic example to demonstrate creating an entity,
    // adding a component to it, and retrieving the component.
    // Explore further to manage entities and components dynamically in your applications.

    return 0;
}
```

Please check out the examples in the ```/examples``` directory for more examples on how to use this library.

## EasyS Integration Guide

This section provides instructions for integrating EasyS into your project using either CMake or direct inclusion.

### Using CMake

EasyS supports CMake integration, allowing you to easily add it to your CMake-based projects.

#### As a Subdirectory

If you've cloned or downloaded EasyS into your project's directory structure, you can add it as a subdirectory in your `CMakeLists.txt`:

```cmake
add_subdirectory(path/to/easys)
target_link_libraries(your_target_name INTERFACE easys)
```

Replace `path/to/easys` with the actual path to EasyS within your project, and `your_target_name` with the name of your project's target.

#### Using FetchContent (CMake 3.11+)

CMake's FetchContent module allows you to dynamically download and include libraries directly in your build process. To integrate EasyS using FetchContent:

```cmake
include(FetchContent)
FetchContent_Declare(
  easys
  GIT_REPOSITORY https://github.com/raphaelmayer/easys.git
  GIT_TAG main # or any version tag you prefer
)
FetchContent_MakeAvailable(easys)
target_link_libraries(your_target_name INTERFACE easys)
```

### Direct Inclusion

For projects not using CMake or for those preferring a simpler integration method, EasyS can be directly included in your project.

1. **Copy or Clone EasyS**: Place the `include` directory of EasyS into your project's directory structure.

2. **Update Include Paths**: Ensure your compiler includes the path to EasyS. This step varies depending on your development environment and build system. Generally, you'll need to add the path to EasyS's `include` directory to your compiler's include directories.

3. **Include the Headers**: In your source files, directly include the header file from EasyS:

```cpp
#include "easys.hpp"
```

## Support

If you encounter any issues or have questions regarding the integration process, please feel free to open an issue on the [EasyS GitHub repository](https://github.com/yourusername/EasyS/issues).
