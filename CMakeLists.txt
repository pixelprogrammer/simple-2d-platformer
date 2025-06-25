cmake_minimum_required(VERSION 3.15)
project(simple-2d-platformer C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compile the commands file
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find raylib
find_package(raylib QUIET)

if (NOT raylib_FOUND)
    # Fetch raylib from GitHub if not found
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.0
    )
    FetchContent_MakeAvailable(raylib)
endif()

# Collect all .c files in src directory
file(GLOB_RECURSE SOURCES "src/*.c")

# Collect all .h files in src directory for IDE support
file(GLOB_RECURSE HEADERS "src/*.h")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link raylib
target_link_libraries(${PROJECT_NAME} raylib)

# Include src directory for headers
target_include_directories(${PROJECT_NAME} PRIVATE src)

# Platform-specific settings
if(UNIX AND NOT APPLE)
    # Linux
    target_link_libraries(${PROJECT_NAME} m pthread dl rt)
endif()

if(APPLE)
    # macOS
    target_link_libraries(${PROJECT_NAME} "-framework CoreVideo" "-framework IOKit" "-framework Cocoa" "-framework GLUT" "-framework OpenGL")
endif()

if(WIN32)
    # Windows
    target_link_libraries(${PROJECT_NAME} winmm)
endif()
