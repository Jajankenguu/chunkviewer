cmake_minimum_required(VERSION 3.24)
project(chunkviewer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(chunkviewer_warnings INTERFACE)
target_compile_options(chunkviewer_warnings INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /permissive->
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>)

include(FetchContent)
FetchContent_Declare(raylib URL
https://github.com/raysan5/raylib/archive/refs/tags/6.0.tar.gz SYSTEM)
FetchContent_MakeAvailable(raylib)

file(GLOB APP_SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(app ${APP_SOURCES})
target_link_libraries(app PRIVATE raylib chunkviewer_warnings)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT app)
