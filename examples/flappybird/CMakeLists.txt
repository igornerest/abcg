project(flappybird)

add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp pipes.cpp bird.cpp)

enable_abcg(${PROJECT_NAME})
