cmake_minimum_required(VERSION 3.15)
project(minesweeper)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 17)

add_executable(minesweeper minesweeper.cpp config/Mode.cpp config/Options.cpp config/Options.h config/Layout.cpp config/Layout.h util/ClockTimer.cpp util/ClockTimer.h util/Random.cpp util/Random.h)
target_link_libraries(minesweeper ${SDL2_LIBRARIES})