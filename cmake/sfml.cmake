set(SFML_ROOT_DIR "" CACHE PATH "SFML source root directory")
if(NOT SFML_ROOT_DIR)
  message(FATAL_ERROR "SFML not found. Please set SFML_ROOT_DIR.")
endif()

include_directories("${SAGE_SFML_ROOT_DIR}/include")
include_directories("${SAGE_SFML_ROOT_DIR}/src")

# Make the GLEW headers available.
include_directories("${GLEW_INCLUDE_PATH}")
# We have to define this to make us link with the GLEW that is in the SFML libs.
add_definitions("-DGLEW_STATIC")
