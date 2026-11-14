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
FetchContent_Declare(imgui URL
https://github.com/ocornut/imgui/archive/refs/tags/v1.92.4.tar.gz
SOURCE_SUBDIR not-a-cmake-project)
FetchContent_Declare(rlimgui GIT_REPOSITORY
https://github.com/raylib-extras/rlImGui.git GIT_TAG dc7f97679a02
SOURCE_SUBDIR not-a-cmake-project)
FetchContent_MakeAvailable(raylib doctest imgui rlimgui)

add_library(imgui STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp ${imgui_SOURCE_DIR}/imgui_demo.cpp)
target_include_directories(imgui SYSTEM PUBLIC ${imgui_SOURCE_DIR})

add_library(rlimgui STATIC ${rlimgui_SOURCE_DIR}/rlImGui.cpp)
target_include_directories(rlimgui SYSTEM PUBLIC ${rlimgui_SOURCE_DIR})
target_link_libraries(rlimgui PUBLIC raylib imgui)

file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS src/lib/*.cpp)
add_library(chunkviewer STATIC ${LIB_SOURCES})
target_include_directories(chunkviewer PUBLIC include)
target_link_libraries(chunkviewer PUBLIC chunkviewer_warnings)

file(GLOB APP_SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(app ${APP_SOURCES})
target_link_libraries(app PRIVATE chunkviewer rlimgui)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT app)

enable_testing()
add_executable(chunkviewer_tests tests/test_world.cpp)
target_link_libraries(chunkviewer_tests PRIVATE  chunkviewer doctest::doctest)
add_test(NAME world COMMAND chunkviewer_tests)
