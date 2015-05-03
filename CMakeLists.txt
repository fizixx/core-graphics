cmake_minimum_required(VERSION 2.8)

project("core-graphics")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/out")

include("cmake/nucleus.cmake")
include("cmake/sfml.cmake")

add_subdirectory(${SFML_ROOT_DIR} "sfml")
include_directories("${SFML_ROOT_DIR}/include")
include_directories("${SFML_ROOT_DIR}/src")
include_directories("${SFML_ROOT_DIR}/extlibs/headers")

add_subdirectory("third_party/glm" "glm")
include_directories("${glm_SOURCE_DIR}")

# GLEW
set("glew_INCLUDE_DIRS" "${CMAKE_SOURCE_DIR}/third_party/glew-1.12.0/include")
set("glew_LIBRARIES" "${CMAKE_SOURCE_DIR}/third_party/glew-1.12.0/lib/Release/x64/glew32s.lib")
include_directories(${glew_INCLUDE_DIRS})
add_definitions("-D GLEW_STATIC=1")

set("common_SOURCE_FILES"
  "src/shader.cpp"
  "src/shader.h"
  "src/utils.cpp"
  "src/utils.h"
  "src/vertex_buffer_object.cpp"
  "src/vertex_buffer_object.h"
)

add_executable("core-graphics" WIN32 MACOSX_BUNDLE
  "src/main.cpp"
  ${common_SOURCE_FILES}
)
target_link_libraries("core-graphics"
  "nucleus"
  "sfml-graphics"
  "sfml-window"
  "sfml-system"
  "sfml-main"
  ${glew_LIBRARIES}
)

add_executable("minimal" WIN32 MACOSX_BUNDLE
  "src/minimal.cpp"
  ${common_SOURCE_FILES}
)
target_link_libraries("minimal"
  "nucleus"
  "sfml-graphics"
  "sfml-window"
  "sfml-system"
  "sfml-main"
  ${glew_LIBRARIES}
)
