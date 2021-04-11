project(maze3d)
add_executable(${PROJECT_NAME} main.cpp model.cpp openglwindow.cpp camera.cpp  maze.cpp)
enable_abcg(${PROJECT_NAME})
