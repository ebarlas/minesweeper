cmake_minimum_required(VERSION 3.15)
project(minesweeper)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 17)

set(SDL2Framework
    SDL2/framework/IWindow.h
    SDL2/framework/IRenderer.h
    SDL2/framework/IImageRepo.h
    SDL2/framework/ITexture.h
    )
set(SDL2Detail
    SDL2/detail/SDLWrapper.h
    SDL2/detail/SDLWrapper.cpp
    SDL2/detail/Window.h
    SDL2/detail/Window.cpp
    SDL2/detail/Renderer.h
    SDL2/detail/Renderer.cpp
    SDL2/detail/ImageRepo.h
    SDL2/detail/ImageRepo.cpp
    SDL2/detail/Texture.h
    SDL2/detail/Texture.cpp
    )

add_executable(minesweeper
    minesweeper.cpp
    NewMinesweeper.cpp
    GameContext.h
    DigitalPanel.h
    Layout.h
    Layout.cpp
    Mode.h
    Mode.cpp
    Options.h
    Options.cpp
    Sprite.h
    Sprite.cpp
    ${SDL2Framework}
    ${SDL2Detail}
    )
target_link_libraries(minesweeper ${SDL2_LIBRARIES})
