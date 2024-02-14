# tests

include(FetchContent)

# Testing library
FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.13.6)
FetchContent_MakeAvailable(catch)

add_executable(tests main.test.cpp)
target_link_libraries(tests PRIVATE ${PROJECT_NAME} Catch2::Catch2)
add_test(NAME test COMMAND tests)

add_executable(benchmarks "ecs.benchmark.cpp")
target_link_libraries(benchmarks PRIVATE ${PROJECT_NAME} Catch2::Catch2)
add_test(NAME benchmark COMMAND benchmarks)
