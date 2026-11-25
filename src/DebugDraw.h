#pragma once

#include <SDL3/SDL.h>

#include "Math.h"

void RenderWorldRectOutline(
    SDL_Renderer* renderer,
    Math::Rect const &rect,
    Math::Camera2D const &camera,
    SDL_Color const &color);

class DebugWorldMarker
{
public:
    void Render(SDL_Renderer *renderer, Math::Camera2D const &camera) const;
};

class DebugWorldGrid
{
    Math::Vector2 area_size{};
    
public:
    void SetWorldSize(Math::Vector2 const &world_size) {area_size = world_size;}
    void Render(SDL_Renderer *renderer, Math::Camera2D const &camera) const;
};