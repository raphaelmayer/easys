# examples

add_executable(basic "basic.cpp")
target_link_libraries(basic PRIVATE ${PROJECT_NAME})
add_test(NAME basic COMMAND basic)

add_executable(basic2 "basic2.cpp")
target_link_libraries(basic2 PRIVATE ${PROJECT_NAME})
add_test(NAME basic2 COMMAND basic2)

add_executable(simple_gameloop "simple_gameloop.cpp")
target_link_libraries(simple_gameloop PRIVATE ${PROJECT_NAME})
add_test(NAME simple_gameloop COMMAND simple_gameloop)