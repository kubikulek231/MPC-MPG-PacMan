cmake_minimum_required(VERSION 3.28)
project(glft2)

# Collect header and source files
file(GLOB_RECURSE glft2_sources src/*.cpp include/glft2/*.hpp)

# Create static library
add_library(glft2_lib STATIC ${glft2_sources})

# Set include directories — use the variables passed from the parent
target_include_directories(glft2_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${GLUT_INCLUDE_DIR}
    ${FREETYPE_INCLUDE_DIRS}
)

# Link with FreeType and FreeGLUT passed in from the parent
target_link_libraries(glft2_lib
    PUBLIC
    ${FREETYPE_LIB}
    "${GLUT_LIB_DIR}/freeglut.lib"
)

# Optional compile flags
target_compile_options(glft2_lib PUBLIC)
