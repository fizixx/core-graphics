cmake_minimum_required(VERSION 2.8)

project("core-graphics")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/out")

include("cmake/sfml.cmake")

add_subdirectory(${SFML_ROOT_DIR} "sfml")
include_directories("${SFML_ROOT_DIR}/include")
include_directories("${SFML_ROOT_DIR}/src")
include_directories("${SFML_ROOT_DIR}/extlibs/headers")

add_subdirectory("third_party/glm" "glm")
include_directories("${glm_SOURCE_DIR}")

add_executable("core-graphics" "src/main.cpp")
target_link_libraries("core-graphics"
  "sfml-system"
  "sfml-window"
  "sfml-graphics"
  "${SFML_ROOT_DIR}/extlibs/libs-msvc/x64/glew.lib"
)
