cmake_minimum_required(VERSION 3.15)
project(minesweeper)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 17)

add_executable(minesweeper minesweeper.cpp Mode.cpp Options.cpp Options.h Layout.cpp Layout.h ClockTimer.cpp ClockTimer.h Random.cpp Random.h)
target_link_libraries(minesweeper ${SDL2_LIBRARIES})