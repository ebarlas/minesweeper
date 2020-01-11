#pragma once

#include "../framework/IImageRepo.h"

#include <map>

namespace sdl {

class ImageRepo : public IImageRepo
{
public:
    using ITexturePtr = std::shared_ptr<ITexture>;
    using Images = std::map<std::string, ITexturePtr>;
    using RendererWPtr = std::weak_ptr<class Renderer>;

public:
    ImageRepo(const std::string &folder, const RendererWPtr& renderer)
        : _folder(folder), _renderer(renderer)
    {}

public:
    ITexturePtr get(const std::string &name) override;

private:
    ITexturePtr load(const std::string &name);

private:
    const std::string _folder;
    const RendererWPtr _renderer;

    mutable Images _images;
};

} // namespace sdl
