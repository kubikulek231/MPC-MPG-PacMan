cmake_minimum_required(VERSION 3.28)
project(MpgProject)

# Restore NuGet packages
execute_process(
    COMMAND nuget restore ${CMAKE_SOURCE_DIR}/Project.sln
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

message("Cmake source directory: "${CMAKE_SOURCE_DIR})

# Set the cache file directory (for CMakeCache.txt location)
set(CMAKE_CACHEFILE_DIR "${CMAKE_SOURCE_DIR}")

# Ensure that CMake runs each time before building
set(CMAKE_SUPPRESS_REGENERATION 0)

# Locate NuGet-installed GLUT
set(NUGET_PACKAGE_PATH "${CMAKE_SOURCE_DIR}/packages")
set(GLUT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/build/packages/nupengl.core.0.1.0.1/build/native/include")
set(GLUT_LIB_DIR "${CMAKE_SOURCE_DIR}/build/packages/nupengl.core.0.1.0.1/build/native/lib/x64")
set(GLUT_DLL_DIR "${CMAKE_SOURCE_DIR}/build/packages/nupengl.core.redist.0.1.0.1/build/native/bin/x64")

# Manually specify NuGet-installed FreeType paths
set(FREETYPE_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/build/packages/freetype.2.8.0.1/build/native/include")
set(FREETYPE_LIB_DIR "${CMAKE_SOURCE_DIR}/build/packages/freetype.2.8.0.1/build/native/lib/x64/v141/dynamic/Release")
set(FREETYPE_LIB "${FREETYPE_LIB_DIR}/freetype28.lib")
set(FREETYPE_DLL "${CMAKE_SOURCE_DIR}/build/packages/freetype.redist.2.8.0.1/build/native/bin/x64/v141/dynamic/Release/freetype28.dll")

# Setup glft2 library (custom GL font loading and rendering lib using freetype fonts)
# Based on https://github.com/benhj/glfreetype
set(GLFT2_LIB_DIR "${CMAKE_SOURCE_DIR}/lib/glft2")
add_subdirectory("${GLFT2_LIB_DIR}")

# Include directories (including GLUT and Freetype)
include_directories(
    ${GLUT_INCLUDE_DIR}          # Include GLUT headers for the whole project
    ${FREETYPE_INCLUDE_DIRS}     # Include FreeType headers for the whole project
    ${CMAKE_SOURCE_DIR}/include  # Your project headers
)

# Link directories (where the libraries are located)
link_directories(${GLUT_LIB_DIR} ${FREETYPE_LIB_DIR})

# Collect all .cpp and .h source files
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.h")

# Add executable
add_executable(MPG ${SOURCES} ${HEADERS})

# Link Freeglut, glft2_lib to the main program
target_link_libraries(MPG glft2_lib "${GLUT_LIB_DIR}/freeglut.lib")

# Message for binary directory
message(STATUS "CMake binary directory: ${CMAKE_BINARY_DIR}")

# Copy freeglut.dll to the output directory
add_custom_command(TARGET MPG POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${GLUT_DLL_DIR}/freeglut.dll"
    ${CMAKE_BINARY_DIR}
)

# Copy Freetype dll
add_custom_command(TARGET MPG POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${FREETYPE_DLL}"
    ${CMAKE_BINARY_DIR}
)

# Copy maps dir to the output directory
add_custom_command(TARGET MPG POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets/"
    "${CMAKE_BINARY_DIR}/assets/"
)
