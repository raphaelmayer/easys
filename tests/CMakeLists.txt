# Testing library
FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.13.6)
FetchContent_MakeAvailable(catch)

add_executable(tests main.test.cpp
	# add other source files here
)
target_include_directories(tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(tests PUBLIC Catch2::Catch2)
add_test(NAME test COMMAND tests)

add_executable(benchmarks ECSManager.benchmark.cpp)
target_include_directories(benchmarks PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(benchmarks PUBLIC Catch2::Catch2)
add_test(NAME benchmark COMMAND benchmarks)
