cmake_minimum_required(VERSION 3.15)
project(minesweeper)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 17)

add_executable(
        minesweeper
        minesweeper.cpp
        config/Mode.cpp
        config/Options.cpp
        config/Layout.cpp
        util/ClockTimer.cpp
        util/Random.cpp
        sdl/Texture.cpp
        sdl/ImageRepo.cpp
        sdl/Renderer.cpp
        sprite/Sprite.cpp
        sprite/DigitPanel.cpp
        sprite/Timer.cpp)
target_link_libraries(minesweeper ${SDL2_LIBRARIES})