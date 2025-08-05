# Minimum required CMake version
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# Project name for the editor application
project(SquidlInk_Editor CXX)

# Set the C++ standard (should match the library's standard)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Define common paths relative to this CMakeLists.txt ---
# Path to the root of the Squidl project
# This assumes SquidlInk is at Project/tools/SquidlInk
set(SQUIDL_PROJECT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../") # Go up two levels from tools/SquidlInk

set(SQUIDL_LIB_INCLUDE_DIR "${SQUIDL_PROJECT_ROOT_DIR}/include")
set(SQUIDL_LIB_BUILD_DIR "${SQUIDL_PROJECT_ROOT_DIR}/build") # Assumes Squidl library is built here

# Source file for the SquidlInk editor
set(SQUIDL_INK_SOURCES
    src/main.cpp
    # Add any other .cpp files specific to SquidlInk editor application here
    # e.g., src/EditorWindow.cpp
    #       src/DesignPanel.cpp
)

# --- Add the SquidlInk executable ---
add_executable(SquidlInk ${SQUIDL_INK_SOURCES})

# --- Include directories for SquidlInk ---
target_include_directories(SquidlInk PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"     # For its own headers if any (e.g., src/EditorWindow.h)
    "${SQUIDL_LIB_INCLUDE_DIR}"           # For Squidl.h and SquidlConfig.h
    "${SQUIDL_PROJECT_ROOT_DIR}/squidl"   # For internal Squidl headers like "Squidl/elements/Button.h"
)

# --- Link SquidlInk to the Squidl library ---
# Option 1: Link directly to the built Squidl library
# This assumes Squidl has been built and its .lib/.so/.dylib file is in the build directory.
target_link_directories(SquidlInk PRIVATE
    "${SQUIDL_LIB_BUILD_DIR}"
)
target_link_libraries(SquidlInk PRIVATE squidl) # SquidlInk depends on Squidl library

# Option 2: Add Squidl as a subdirectory (if you want to build Squidl and SquidlInk together)
# This would go in the Project root CMakeLists.txt, not here.
# For now, we'll stick to Option 1 for this temporary SquidlInk CMakeLists.txt.

# --- Copy assets for the editor (if SquidlInk uses specific assets) ---
# This is an example; adjust as needed.
# If SquidlInk uses assets from the main assets/ folder, you might not need this.
# if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources")
#     file(GLOB_RECURSE SQUIDL_INK_RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/resources/*")
#     add_custom_command(TARGET SquidlInk POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy_directory
#         "${CMAKE_CURRENT_SOURCE_DIR}/resources"
#         "$<TARGET_FILE_DIR:SquidlInk>/resources"
#         COMMENT "Copying SquidlInk resources"
#     )
# endif()