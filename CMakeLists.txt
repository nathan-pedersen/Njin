cmake_minimum_required(VERSION 3.10)
project(Main)

# set c++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# export compile commands for LSP to recognize c++ standard
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# find and use SFML 2 with components
find_package(SFML 2 REQUIRED COMPONENTS graphics audio system)

# find and use OpenGL
find_package(OpenGL REQUIRED)

# compile imgui as a static library
add_library(imgui
            ./imgui/imgui.cpp
            ./imgui/imgui_draw.cpp
            ./imgui/imgui_tables.cpp
            ./imgui/imgui_widgets.cpp
)
target_include_directories(imgui PUBLIC ./imgui/)

# compile imgui-sfml as a static library (relies on stuff from imgui)
add_library(imgui-sfml
            ./imgui-sfml/imgui-SFML.cpp
)
target_include_directories(imgui-sfml PUBLIC ./imgui-sfml/ ./imgui/)

# link imgui-sfml with SFML and OpenGL
target_link_libraries(imgui-sfml PUBLIC imgui sfml-graphics sfml-system OpenGL::GL)

# other headers
include_directories(Main PUBLIC ./nlohmann)

# my program executable
add_executable(Main 
    ./src/Main.cpp 
    ./src/Vec2.cpp
    ./src/Entity.cpp
    ./src/EntityFactory.cpp
    ./src/Game.cpp
)

target_compile_options(Main PRIVATE
  $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:
    -Wall
    -Wextra
    -Wreturn-type
    -Werror
  >
)

# my headers
target_include_directories(Main PRIVATE ./include/)

# link my executable with SFML, imgui and OpenGL
target_link_libraries(Main PRIVATE sfml-graphics sfml-audio sfml-system imgui-sfml OpenGL::GL)

