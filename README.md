# EasyS

EasyS is a no dependencies header-only Entity-Component-System library written in C++.

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
