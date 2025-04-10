cmake_minimum_required(VERSION 3.28)
project(MpgProject)

# Find OpenGL package
find_package(OpenGL REQUIRED)

# Restore NuGet packages
execute_process(
    COMMAND nuget restore ${CMAKE_SOURCE_DIR}/Project.sln
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# Set the cache file directory (for CMakeCache.txt location)
set(CMAKE_CACHEFILE_DIR "${CMAKE_SOURCE_DIR}")

# Ensure that CMake runs each time before building
set(CMAKE_SUPPRESS_REGENERATION 0)

# Locate NuGet-installed GLUT
set(NUGET_PACKAGE_PATH "${CMAKE_SOURCE_DIR}/packages")
set(GLUT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/build/packages/nupengl.core.0.1.0.1/build/native/include")
set(GLUT_LIB_DIR "${CMAKE_SOURCE_DIR}/build/packages/nupengl.core.0.1.0.1/build/native/lib/x64")
set(GLUT_DLL_DIR "${CMAKE_SOURCE_DIR}/build/packages/nupengl.core.redist.0.1.0.1/build/native/bin/x64")

# Include directories
include_directories(${GLUT_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/include)
link_directories(${GLUT_LIB_DIR})

# Collect all .cpp source files
file(GLOB SOURCES "src/*.cpp")

# Collect all .h header files
file(GLOB HEADERS "include/*.h")

# Add executable
add_executable(MPG ${SOURCES} ${HEADERS})

# Message for binary directory
message(STATUS "CMake binary directory: ${CMAKE_BINARY_DIR}")

# Link libraries
target_link_libraries(MPG OpenGL::GL "${GLUT_LIB_DIR}/freeglut.lib")

# Copy freeglut.dll to the output directory (where the executable is built)
add_custom_command(TARGET MPG POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${GLUT_DLL_DIR}/freeglut.dll"
    ${CMAKE_BINARY_DIR}  # Copies to the output directory of the executable
)

# Copy maps dir to the output directory
add_custom_command(TARGET MPG POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets/maps"
    "${CMAKE_BINARY_DIR}/assets/maps"
)