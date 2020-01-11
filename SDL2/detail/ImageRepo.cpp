#include "ImageRepo.h"
#include "Texture.h"
#include "Renderer.h"

#include "SDL.h"

namespace sdl {

ITexturePtr ImageRepo::get(const std::string &name)
{
    auto texture = _images[name];
    if (texture == nullptr) {
        texture = load(name);
        _images[name] = texture;
    }
    return texture;
}

ITexturePtr ImageRepo::load(const std::string &name)
{
    std::string file{_folder};
    file.append(name);
    file.append(".bmp");
    auto renderer{_renderer.lock()};
    if (!renderer)
        return nullptr;

    SDL_Surface *image = SDL_LoadBMP(file.c_str());
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer->getsdl(), image);
    SDL_FreeSurface(image);
    return std::make_shared<Texture>(texture, _renderer);
}

} // namespace sdl
