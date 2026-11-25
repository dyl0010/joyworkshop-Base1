#pragma once

#include <SDL3/SDL.h>

#include "Entity.h"
#include "Math.h"

class Collectible : public Entity
{
    SDL_Color color{};
    SDL_Color debug_bounds_color{};

    bool collected{false};

public:
    Collectible(Math::Transform const &transform, SDL_Color const &color, SDL_Color const &debug_bounds_color, char const *debug_name);
    void Update(double frame_delta) override;
    void Render(SDL_Renderer* renderer, Math::Camera2D const& camera) const override;

    SDL_Color const &GetDebugBoundsColor() const;

    // Collection is one-shot; collected items become inactive.
    bool IsCollected() const;
    void Collect();

    bool CanRender() const override;
    bool CanRenderDebugBounds() const override;
    bool CanCheckCollision() const override;
    bool CanCheckTrigger() const override;
};
