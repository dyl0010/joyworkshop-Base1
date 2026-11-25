#include "Resource.h"

#include <cassert>
#include <cstdio>
#include <utility>

TextureResource::~TextureResource()
{
    Reset();
}

void TextureResource::Reset()
{
    if (texture)
    {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    width = 0;
    height = 0;
    path.clear();
}

TextureResource::TextureResource(TextureResource&& texture_resource)
{
    *this = std::move(texture_resource);
}

TextureResource& TextureResource::operator=(TextureResource&& texture_resource)
{
    if (&texture_resource != this)
    {
        Reset();

        path = std::move(texture_resource.path);
        texture = texture_resource.texture;
        width = texture_resource.width;
        height = texture_resource.height;

        texture_resource.path.clear();
        texture_resource.texture = nullptr;
        texture_resource.width = 0;
        texture_resource.height = 0;
    }

    return *this;
}

void TextureResource::LogResourceError(char const* action) const
{
    std::fprintf(stderr, "[resource] %s failed: %s\n", action, SDL_GetError());
}

bool TextureResource::LoadTextureBMP(std::string const& new_path, SDL_Renderer* renderer)
{
    if (texture)
    {
        Reset();
    }

    assert(renderer);

    path = new_path;

    SDL_Surface* surface = SDL_LoadBMP(path.c_str());
    if (surface == nullptr)
    {
        LogResourceError("SDL_LoadBMP");
        return false;
    }

    width = surface->w;
    height = surface->h;

    SDL_Texture* new_texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (new_texture == nullptr)
    {
        SDL_DestroySurface(surface);
        LogResourceError("SDL_CreateTextureFromSurface");
        return false;
    }

    SDL_DestroySurface(surface);
    texture = new_texture;
    return true;
}

SDL_Texture* TextureResource::GetTexture() const
{
    assert(texture != nullptr);
    return texture;
}
