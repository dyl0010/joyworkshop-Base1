#pragma once

#include <SDL3/SDL.h>

#include "Entity.h"
#include "Math.h"

class Grenade : public Entity
{
    float radius{96.f};
    double fuse_seconds{1.0};
    double elapsed{};
    double explosion_visible_seconds{0.2};
    double explosion_elapsed{};
    bool exploded{};
    bool finished{};

    SDL_Color color{};
    SDL_Color debug_bounds_color{};

    Math::Vector2 target_position{};
    float travel_speed{500.f};
    bool landed{};

    SDL_Color warning_color{255, 220, 0, 255};
    double blink_interval{0.15};

public:
    Grenade(Math::Transform const &transform,
            Math::Vector2 const &target_position,
            SDL_Color const &color,
            SDL_Color const &debug_bounds_color,
            char const *debug_name);

    void Update(double frame_delta) override;
    void Render(SDL_Renderer* renderer, Math::Camera2D const& camera) const override;

    bool IsFinished() const;
};