# Minimum required CMake version
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# Project name
project(SquidlUI_Library CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Configuration for building a shared library (DLL/so/dylib) ---
set(BUILD_SHARED_LIBS ON)

# --- Define common paths ---
set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SQUIDL_LIB_SRC_DIR ${PROJECT_ROOT_DIR}/squidl) # Path to Squidl library source (where .cpp files are)
set(ASSETS_DIR ${PROJECT_ROOT_DIR}/assets)
set(INCLUDE_DIR ${PROJECT_ROOT_DIR}/include) # Path to Squidl public headers (where include/Squidl is)
set(SAMPLES_DIR ${PROJECT_ROOT_DIR}/samples)

# --- Find SDL2 and its components ---
find_package(PkgConfig QUIET)
if (PKG_CONFIG_FOUND)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
    pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
    # Add SDL2_gfx
    pkg_check_modules(SDL2_GFX REQUIRED SDL2_gfx)
endif()

if (NOT SDL2_FOUND OR NOT SDL2_IMAGE_FOUND OR NOT SDL2_TTF_FOUND OR NOT SDL2_GFX_FOUND)
    # Option 2: Manual SDL2 paths (Adjust these to your SDL2 installation)
    # Example for macOS with Homebrew:
    set(SDL2_INCLUDE_DIR "/opt/homebrew/include/SDL2")
    set(SDL2_LIBRARY_DIR "/opt/homebrew/lib")

    # Example for Linux (adjust if different):
    # set(SDL2_INCLUDE_DIR "/usr/include/SDL2")
    # set(SDL2_LIBRARY_DIR "/usr/lib")

    # Example for Windows (adjust if different):
    # set(SDL2_INCLUDE_DIR "C:/Libraries/SDL2/include")
    # set(SDL2_LIBRARY_DIR "C:/Libraries/SDL2/lib/x64")
    # set(SDL2_GFX_INCLUDE_DIR "C:/Libraries/SDL2_gfx/include")
    # set(SDL2_GFX_LIBRARY_DIR "C:/Libraries/SDL2_gfx/lib/x64")

    find_path(SDL2_INCLUDE_DIR SDL.h HINTS ${SDL2_INCLUDE_DIR} PATH_SUFFIXES SDL2)
    find_library(SDL2_LIBRARY SDL2 HINTS ${SDL2_LIBRARY_DIR})
    find_library(SDL2MAIN_LIBRARY SDL2main HINTS ${SDL2_LIBRARY_DIR})

    # For SDL2_image
    find_path(SDL2_IMAGE_INCLUDE_DIR SDL_image.h HINTS ${SDL2_INCLUDE_DIR} PATH_SUFFIXES SDL2)
    find_library(SDL2_IMAGE_LIBRARY SDL2_image HINTS ${SDL2_LIBRARY_DIR})

    # For SDL2_ttf
    find_path(SDL2_TTF_INCLUDE_DIR SDL_ttf.h HINTS ${SDL2_INCLUDE_DIR} PATH_SUFFIXES SDL2)
    find_library(SDL2_TTF_LIBRARY SDL2_ttf HINTS ${SDL2_LIBRARY_DIR})
    
    # For SDL2_gfx
    find_path(SDL2_GFX_INCLUDE_DIR SDL2_gfxPrimitives.h HINTS ${SDL2_INCLUDE_DIR} PATH_SUFFIXES SDL2_gfx)
    find_library(SDL2_GFX_LIBRARY SDL2_gfx HINTS ${SDL2_LIBRARY_DIR})

    if (NOT SDL2_INCLUDE_DIR OR NOT SDL2_LIBRARY OR NOT SDL2_IMAGE_LIBRARY OR NOT SDL2_TTF_LIBRARY OR NOT SDL2_GFX_LIBRARY)
        message(FATAL_ERROR "Could not find all SDL2 components. Please adjust SDL2_INCLUDE_DIR and SDL2_LIBRARY_DIR in CMakeLists.txt.")
    endif()

    set(SDL2_LIBRARIES ${SDL2_LIBRARY} ${SDL2MAIN_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY} ${SDL2_GFX_LIBRARY})
    set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIR} ${SDL2_GFX_INCLUDE_DIR})
endif()

# --- Define all source files for the Squidl library ---
# Ensure 'Squidl' (with capital S) is the actual directory name where your .cpp files reside.
file(GLOB_RECURSE SQUIDL_SOURCES
    "${SQUIDL_LIB_SRC_DIR}/base/*.cpp"      # Assuming UIElement.cpp, etc.
    "${SQUIDL_LIB_SRC_DIR}/core/*.cpp"      # Assuming UIContext.cpp, etc.
    "${SQUIDL_LIB_SRC_DIR}/editor/*.cpp"    # Assuming editor files like Editor.cpp, etc.
    "${SQUIDL_LIB_SRC_DIR}/elements/*.cpp"  # Assuming Button.cpp, Label.cpp, Input.cpp, Backdrop.cpp
    "${SQUIDL_LIB_SRC_DIR}/layouts/*.cpp"   # Assuming Layout.cpp, HBoxLayout.cpp, etc.
    "${SQUIDL_LIB_SRC_DIR}/managers/*.cpp"  # Assuming managers .cpp files
    "${SQUIDL_LIB_SRC_DIR}/renderers/*.cpp" # <--- ДОБАВЬТЕ ЭТУ СТРОКУ для новой директории renderers
    "${SQUIDL_LIB_SRC_DIR}/utils/*.cpp"     # Assuming Logger.cpp, etc. (if they are not header-only)
    "${SQUIDL_LIB_SRC_DIR}/editor/*.cpp"    # Assuming editor core files
)

# --- Add the Squidl library ---
add_library(Squidl ${SQUIDL_SOURCES})

# Define SQUIDL_EXPORTS for Windows DLL builds
if (MSVC)
    target_compile_definitions(Squidl PUBLIC SQUIDL_EXPORTS)
endif()

# Set include directories for the Squidl library itself
target_include_directories(Squidl PUBLIC
    "${INCLUDE_DIR}"
    "${SQUIDL_LIB_SRC_DIR}" # For internal includes like "Squidl/elements/Button.h"
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_CFLAGS_OTHER}
    ${SDL2_IMAGE_CFLAGS_OTHER}
    ${SDL2_TTF_CFLAGS_OTHER}
    ${SDL2_GFX_INCLUDE_DIRS}
)

# Link Squidl library to SDL2 libraries
target_link_libraries(Squidl PUBLIC
    ${SDL2_LIBRARIES}
    ${SDL2_LDFLAGS_OTHER}
    ${SDL2_IMAGE_LDFLAGS_OTHER}
    ${SDL2_TTF_LDFLAGS_OTHER}
    ${SDL2_GFX_LIBRARIES}
)

# --- Add the main application (test app) ---
add_executable(SquidlTestApp ${PROJECT_ROOT_DIR}/src/main.cpp)

# Link the test app to the Squidl library and SDL2
target_link_libraries(SquidlTestApp PRIVATE Squidl)

# Копируем папку assets в директорию сборки (для удобства запуска)
# CMAKE_CURRENT_BINARY_DIR - это текущая директория сборки (например, build/)
file(COPY "${PROJECT_ROOT_DIR}/assets/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")