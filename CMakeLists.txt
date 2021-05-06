cmake_minimum_required(VERSION 3.10)

project(core-graphics)

find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)
# find_package(assimp CONFIG REQUIRED)

find_path(STB_INCLUDE_DIRS "stb.h")

add_executable(core-graphics WIN32 MACOSX_BUNDLE src/main.cpp)
target_link_libraries(core-graphics PRIVATE glfw GLEW::GLEW glm)  # assimp::assimp
target_include_directories(core-graphics PRIVATE ${STB_INCLUDE_DIRS})
set_target_properties(core-graphics PROPERTIES
        CXX_STANDARD 17
        )
