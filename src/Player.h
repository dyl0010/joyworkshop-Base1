#pragma once

#include <SDL3/SDL.h>

#include <array>

#include "Direction.h"
#include "Movement.h"
#include "InputManager.h"
#include "Math.h"
#include "SpriteAnimation.h"
#include "Entity.h"

class TextureResource;

class Player : public Entity
{
    TextureResource const* texture_resource = nullptr;
    SDL_FRect sprite_rect{};
    double animation_rate{};

    int move_speed{};

    float sprite_width{};
    float sprite_height{};

    std::array<std::size_t, 4> animation_indices{};

    SpriteAnimation sprite_animation{};

    Direction facing{Direction::Left};

    Math::Vector2 area_size{};

    Math::Vector2 player_start_point{};

    Math::Rect GetNextBounds(double frame_delta, Math::Vector2 const &velocity, bool apply_x, bool apply_y) const;

public:
    std::array<std::size_t, 4> GetSpriteIndexByDirection(Direction direction) const;
    SDL_FRect GetSpriteRectByIndex(std::size_t index) const;
    
    MovementIntent BuildMovementIntent(double frame_delta, InputManager const& input) const;
    void ApplyMovement(Math::Rect const &bounds);
    void ApplyMovementIntent(MovementIntent const &movement_intent, double frame_delta);
    ThrowGrenadeIntent BuildThrowGrenadeIntent(InputManager const &input, Math::Vector2 const &velocity) const;
    ThrowBombIntent BuildThrowBombIntent(InputManager const &input) const;

    void Initialize(TextureResource const* texture_resource, Math::Vector2 const &world_size);
    void Update(double frame_delta) override;
    void Render(SDL_Renderer* renderer, Math::Camera2D const& camera) const override;

    bool CanCheckTrigger() const override;

    Math::Vector2 GetFacingVector() const;
};
