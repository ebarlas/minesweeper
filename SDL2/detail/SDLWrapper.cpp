#include "SDLWrapper.h"

namespace sdl {

// free functions

void SDLRelease(SDL_Window *window)
{
    SDL_DestroyWindow(window);
}

void SDLRelease(SDL_Renderer *renderer)
{
    SDL_DestroyRenderer(renderer);
}

void SDLRelease(SDL_Texture *texture)
{
    SDL_DestroyTexture(texture);
}

} // namespace sdl
