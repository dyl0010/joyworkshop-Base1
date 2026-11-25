#pragma once

#include <SDL3/SDL.h>

#include <vector>

#include "Entity.h"
#include "Math.h"

class Bomb : public Entity
{
    double fuse_seconds{1.5};
    double elapsed{};
    double explosion_visible_seconds{0.25};
    double explosion_elapsed{};

    bool exploded{};
    bool finished{};

    int explosion_range{2};
    float tile_size{64.f};

    SDL_Color fuse_color{80, 80, 80, 255};
    SDL_Color warning_color{255, 220, 0, 255};
    SDL_Color explosion_color{255, 100, 0, 255};
    double blink_interval{0.15};

public:
    Bomb(Math::Transform const &transform, char const *debug_name);

    void Update(double frame_delta) override;
    void Render(SDL_Renderer* renderer, Math::Camera2D const& camera) const override;

    bool IsFinished() const;
    bool HasExploded() const;

    std::vector<Math::Rect> GetExplosionBounds() const;
    int GetExplosionRange() const;
};