cmake_minimum_required(VERSION 3.24)
project(chunkviewer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(chunkviewer_warnings INTERFACE)
target_compile_options(chunkviewer_warnings INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /permissive->
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
include(FetchContent)
FetchContent_Declare(raylib URL
https://github.com/raysan5/raylib/archive/refs/tags/6.0.tar.gz SYSTEM)
FetchContent_Declare(doctest URL
https://github.com/doctest/doctest/archive/refs/tags/v2.4.11.tar.gz SYSTEM)
FetchContent_MakeAvailable(raylib doctest)

file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS src/lib/*.cpp)
add_library(chunkviewer STATIC ${LIB_SOURCES})
target_include_directories(chunkviewer PUBLIC include)
target_link_libraries(chunkviewer PUBLIC chunkviewer_warnings)

file(GLOB APP_SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(app ${APP_SOURCES})
target_link_libraries(app PRIVATE chunkviewer raylib chunkviewer_warnings)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT app)

enable_testing()
add_executable(chunkviewer_tests tests/test_world.cpp)
target_link_libraries(chunkviewer_tests PRIVATE  chunkviewer doctest::doctest)
add_test(NAME world COMMAND chunkviewer_tests)
