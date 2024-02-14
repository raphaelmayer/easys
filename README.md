# MyHeaderOnlyLib

MyHeaderOnlyLib is a no dependencies header-only Entity Component System library written in C++.

## MyHeaderOnlyLib Integration Guide

This section provides instructions for integrating MyHeaderOnlyLib into your project using either CMake or direct inclusion.

### Using CMake

MyHeaderOnlyLib supports CMake integration, allowing you to easily add it to your CMake-based projects.

#### As a Subdirectory

If you've cloned or downloaded MyHeaderOnlyLib into your project's directory structure, you can add it as a subdirectory in your `CMakeLists.txt`:

```cmake
add_subdirectory(path/to/MyHeaderOnlyLib)
target_link_libraries(your_target_name INTERFACE MyHeaderOnlyLib)
```

Replace `path/to/MyHeaderOnlyLib` with the actual path to MyHeaderOnlyLib within your project, and `your_target_name` with the name of your project's target.

#### Using FetchContent (CMake 3.11+)

CMake's FetchContent module allows you to dynamically download and include libraries directly in your build process. To integrate MyHeaderOnlyLib using FetchContent:

```cmake
include(FetchContent)
FetchContent_Declare(
  MyHeaderOnlyLib
  GIT_REPOSITORY https://github.com/yourusername/MyHeaderOnlyLib.git
  GIT_TAG main # or any version tag you prefer
)
FetchContent_MakeAvailable(MyHeaderOnlyLib)
target_link_libraries(your_target_name INTERFACE MyHeaderOnlyLib)
```

### Direct Inclusion

For projects not using CMake or for those preferring a simpler integration method, MyHeaderOnlyLib can be directly included in your project.

1. **Copy or Clone MyHeaderOnlyLib**: Place the `include` directory of MyHeaderOnlyLib into your project's directory structure.

2. **Update Include Paths**: Ensure your compiler includes the path to MyHeaderOnlyLib. This step varies depending on your development environment and build system. Generally, you'll need to add the path to MyHeaderOnlyLib's `include` directory to your compiler's include directories.

3. **Include the Headers**: In your source files, directly include the header files from MyHeaderOnlyLib as needed:

```cpp
#include "MyHeaderOnlyLib/my_header.hpp"
```

Replace `my_header.hpp` with the specific header file(s) you wish to include from MyHeaderOnlyLib.

## Support

If you encounter any issues or have questions regarding the integration process, please feel free to open an issue on the [MyHeaderOnlyLib GitHub repository](https://github.com/yourusername/MyHeaderOnlyLib/issues).

## Misc Notes / TODO

- The registry could record and keep track of all used component types, which would solve a lot of current "issues". ```removeComponents(Entity)``` and ```size()``` would work without requireing the user to specify any types.
- Add a ```getEntitiesWithComponent(s)``` method.
- ```getAllComponentsByType``` should probably return a container of component refs. Then we do not expose internals to the user. 
	Otherwise ```SparseSet``` needs to be improved (i.e. iterator, safety, UX)
	Or we could implement a ```SparseSet::getValues``` method, which directly returns a ref to the dense component array.