# Pokud je to podporováno, povolte Opětovné načítání za provozu pro kompilátory MSVC.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

cmake_minimum_required(VERSION 3.28)
project (MPG-PacMan)

# Include helpers
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(Helpers)

# Set the cache file directory (for CMakeCache.txt location)
set(CMAKE_CACHEFILE_DIR "${CMAKE_SOURCE_DIR}")

# Ensure that CMake runs each time before building
set(CMAKE_SUPPRESS_REGENERATION 0)

# Define deps dir
set(DEPS_DIR "${CMAKE_SOURCE_DIR}/deps")

# Locate GLUT deps
set(GLUT_INCLUDE_DIR "${DEPS_DIR}/nupengl.core.0.1.0.1/build/native/include")
set(GLUT_LIB_DIR "${DEPS_DIR}/nupengl.core.0.1.0.1/build/native/lib/x64")
set(GLUT_DLL_DIR "${DEPS_DIR}/nupengl.core.redist.0.1.0.1/build/native/bin/x64")
set(GLUT_GLFW3_DLL "${GLUT_DLL_DIR}/glfw3.dll")
set(GLUT_GLEW32_DLL "${GLUT_DLL_DIR}/glew32.dll")
set(GLUT_FREEGLUT_DLL "${GLUT_DLL_DIR}/freeglut.dll")

# Locate FreeType deps
set(FREETYPE_INCLUDE_DIRS "${DEPS_DIR}/freetype.2.8.0.1/build/native/include")
set(FREETYPE_LIB_DIR "${DEPS_DIR}/freetype.2.8.0.1/build/native/lib/x64/v141/dynamic/Release")
set(FREETYPE_LIB "${FREETYPE_LIB_DIR}/freetype28.lib")
set(FREETYPE_DLL "${DEPS_DIR}/freetype.redist.2.8.0.1/build/native/bin/x64/v141/dynamic/Release/freetype28.dll")

# Locate SDL3 deps
set(SDL3_INCLUDE_DIRS "${DEPS_DIR}/SDL3_mixer/include/")
set(SDL3_LIB_DIR "${DEPS_DIR}/SDL3_mixer/lib/")
set(SDL3_DLL "${DEPS_DIR}/SDL3_mixer/bin/SDL3.dll")

# Locate SDL3_mixer deps
set(SDL3_MIXER_DLL "${DEPS_DIR}/SDL3_mixer/bin/SDL3_mixer.dll")

# Extract deps
message("Extracting dependencies...")
if (NOT EXISTS "${GLUT_INCLUDE_DIR}") 
    extract_zip("${DEPS_DIR}/nupengl.zip" "${DEPS_DIR}/") 
else()
    message("Deps for NupenGL already extracted.")
endif()

if (NOT EXISTS "${FREETYPE_INCLUDE_DIRS}")
    extract_zip("${DEPS_DIR}/freetype.zip" "${DEPS_DIR}/")
else()
    message("Deps for FreeType already extracted.")
endif()

if (NOT EXISTS "${SDL3_INCLUDE_DIRS}")
    extract_zip("${DEPS_DIR}/SDL3_mixer.zip" "${DEPS_DIR}/")
else()
    message("Deps for SDL3 already extracted.")
endif()

# Setup glft2 library (custom GL font loading and rendering lib using freetype fonts)
# Based on https://github.com/benhj/glfreetype
set(GLFT2_LIB_DIR "${CMAKE_SOURCE_DIR}/lib/glft2")
add_subdirectory("${GLFT2_LIB_DIR}")

# Include directories (including GLUT and Freetype)
include_directories(
    ${GLUT_INCLUDE_DIR}          # Include GLUT headers for the whole project
    ${FREETYPE_INCLUDE_DIRS}     # Include FreeType headers for the whole project
    ${SDL3_INCLUDE_DIRS}         # Include SDL3 headers for the whole project
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/resources
    ${CMAKE_SOURCE_DIR}/lib/stb
)

# Link directories (where the libraries are located)
link_directories(${GLUT_LIB_DIR} ${FREETYPE_LIB_DIR} ${SDL3_LIB_DIR} )

# Collect all .cpp and .h source files
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.h")

# Add stb_image.h to the HEADERS list
file(GLOB STB_HEADERS "lib/stb/stb_image.h")
list(APPEND HEADERS ${STB_HEADERS})

# Windows: compile the resources.rc file
if (WIN32)
    enable_language(RC)
    set(RESOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/resource.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/resource.rc"
    )
endif()

add_executable(MPG-PacMan ${SOURCES} ${HEADERS} ${RESOURCE_FILES})

# Windows: mark the exe as GUI app (won't start console)
if (WIN32)
    set_target_properties(MPG-PacMan PROPERTIES WIN32_EXECUTABLE ON )
endif()

# Link Freeglut, glft2_lib to the main program
target_link_libraries(MPG-PacMan glft2_lib)

# Link the SDL3 libs with mixer
target_link_libraries(MPG-PacMan
    "${SDL3_LIB_DIR}/SDL3.lib"
    "${SDL3_LIB_DIR}/SDL3_mixer.lib"
)

# Message for binary directory
message(STATUS "CMake binary directory: ${CMAKE_BINARY_DIR}")

# Copy freeglut.dll to the output directory
add_custom_command(TARGET MPG-PacMan POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${GLUT_FREEGLUT_DLL}"
    ${CMAKE_BINARY_DIR}
)

# Copy glew32.dll to the output directory
add_custom_command(TARGET MPG-PacMan POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${GLUT_GLEW32_DLL}"
    ${CMAKE_BINARY_DIR}
)

# Copy glfw3.dll to the output directory
add_custom_command(TARGET MPG-PacMan POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${GLUT_GLFW3_DLL}"
    ${CMAKE_BINARY_DIR}
)

# Copy Freetype dll to the output directory
add_custom_command(TARGET MPG-PacMan POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${FREETYPE_DLL}"
    ${CMAKE_BINARY_DIR}
)

# Copy SDL3.dll to the output directory
add_custom_command(TARGET MPG-PacMan POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${SDL3_DLL}"
    ${CMAKE_BINARY_DIR}
)

# Copy SDL3_mixer.dll to the output directory
add_custom_command(TARGET MPG-PacMan POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${SDL3_MIXER_DLL}"
    ${CMAKE_BINARY_DIR}
)

# Copy assets dir to the output directory
add_custom_command(TARGET MPG-PacMan POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets/"
    "${CMAKE_BINARY_DIR}/assets/"
)

# Copy resources dir to the output directory
add_custom_command(TARGET MPG-PacMan POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/resources/"
    "${CMAKE_BINARY_DIR}/resources/"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MPG-PacMan PROPERTY CXX_STANDARD 20)
endif()