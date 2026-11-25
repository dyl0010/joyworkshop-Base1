#pragma once

#include <SDL3/SDL.h>

#include <string>

class TextureResource
{
    std::string path{};
    SDL_Texture* texture = nullptr;
    int width{};
    int height{};

    void LogResourceError(char const* action) const;

public:
    TextureResource() = default;

    TextureResource(TextureResource const&) = delete;
    TextureResource& operator=(TextureResource const&) = delete;

    TextureResource(TextureResource&& texture_resource);
    TextureResource& operator=(TextureResource&& texture_resource);

    ~TextureResource();

    bool LoadTextureBMP(std::string const& path, SDL_Renderer* renderer);
    SDL_Texture* GetTexture() const;
    void Reset();
};
