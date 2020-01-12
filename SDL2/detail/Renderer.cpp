#include "Renderer.h"
#include "ImageRepo.h"


namespace sdl {

IImageRepoPtr Renderer::createImageRepo(const std::string &folder)
{
    return std::make_shared<ImageRepo>(folder, shared_from_this());
}

void Renderer::repaint()
{
    SDL_RenderPresent(getsdl());
}

} // namespace sdl
