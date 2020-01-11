#pragma once

#include "SDL.h"

namespace sdl {

// free functions
void SDLRelease(SDL_Window *window);
void SDLRelease(SDL_Renderer *renderer);
void SDLRelease(SDL_Texture *texture);

template<bool Destroy, typename T>
class ConditionalWrapper
{
public:
    T *getsdl() const { return _obj; }

protected:
    ConditionalWrapper() = default;
    explicit ConditionalWrapper(T *obj) : _obj(obj) {}
    ~ConditionalWrapper()
    {
        if (Destroy)
            sdl::SDLRelease(_obj);
    }

    T *swap(T *replacement)
    {
        auto swap = _obj;
        _obj = replacement;
        return swap;
    }

private:
    T *_obj = nullptr;
};

template<typename T>
using Wrapper = ConditionalWrapper<true, T>;
template<typename T>
using WrapperNoDestroy = ConditionalWrapper<false, T>;

} // namespace sdl
