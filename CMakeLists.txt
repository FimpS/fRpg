cmake_minimum_required(VERSION 3.10)
project(fRpgCrawler C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 99)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/lib)

find_package(raylib REQUIRED)

file(GLOB_RECURSE ENGINE_SRC
    "${PROJECT_SOURCE_DIR}/src/*.c"
    "${PROJECT_SOURCE_DIR}/lib/*.c"
)

list(REMOVE_ITEM ENGINE_SRC
    ${PROJECT_SOURCE_DIR}/src/main.c
    ${PROJECT_SOURCE_DIR}/src/main_editor.c
)

add_library(engine ${ENGINE_SRC})
target_link_libraries(engine raylib m dl pthread)

# Default build target
add_executable(game src/main.c)
target_link_libraries(game engine)

# Optional target (not built by default)
add_executable(editor EXCLUDE_FROM_ALL src/main_editor.c)
target_link_libraries(editor engine)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
