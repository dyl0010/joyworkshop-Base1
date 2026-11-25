#include "Player.h"

#include <cassert>

#include "Config.h"
#include "Resource.h"

std::array<std::size_t, Config::Player::SpriteSheetColumns> Player::GetSpriteIndexByDirection(Direction direction) const
{
    switch (direction)
    {
    case Direction::Up:
        return Config::Player::UpAnimationIndices;
    case Direction::Down:
        return Config::Player::DownAnimationIndices;
    case Direction::Left:
        return Config::Player::LeftAnimationIndices;
    case Direction::Right:
        return Config::Player::RightAnimationIndices;
    default:
        return Config::Player::LeftAnimationIndices;
    }
}

SDL_FRect Player::GetSpriteRectByIndex(std::size_t index) const
{
    std::size_t const column = index % Config::Player::SpriteSheetColumns;
    std::size_t const row = index / Config::Player::SpriteSheetColumns;

    return SDL_FRect{
        static_cast<float>(column) * sprite_width,
        static_cast<float>(row) * sprite_height,
        sprite_width,
        sprite_height
    };
}

Math::Rect Player::GetNextBounds(double frame_delta, Math::Vector2 const &velocity, bool apply_x, bool apply_y) const
{
    Math::Vector2 next_position = transform.position;
    if (apply_x)
    {
        next_position.x += velocity.x * move_speed * static_cast<float>(frame_delta);
    }
    if (apply_y)
    {
        next_position.y += velocity.y * move_speed * static_cast<float>(frame_delta);
    }

    return Math::Rect{next_position, transform.size};
}

MovementIntent Player::BuildMovementIntent(double frame_delta, InputManager const& input) const
{
    Math::Vector2 velocity{};
    bool is_moving = false;

    if (input.IsActionDown(InputAction::MoveUp))
    {
        velocity.y -= 1.0f;
        is_moving = true;
    }
    if (input.IsActionDown(InputAction::MoveDown))
    {
        velocity.y += 1.0f;
        is_moving = true;
    }
    if (input.IsActionDown(InputAction::MoveLeft))
    {
        velocity.x -= 1.0f;
        is_moving = true;
    }
    if (input.IsActionDown(InputAction::MoveRight))
    {
        velocity.x += 1.0f;
        is_moving = true;
    }

    Math::Normalize(velocity);

    MovementIntent movement_intent{};
    movement_intent.velocity = velocity;
    movement_intent.next_bounds = GetNextBounds(frame_delta, velocity, true, true);
    movement_intent.next_bounds_x = GetNextBounds(frame_delta, velocity, true, false);
    movement_intent.next_bounds_y = GetNextBounds(frame_delta, velocity, false, true);
    movement_intent.is_moving = is_moving;
    return movement_intent;
}

ThrowGrenadeIntent Player::BuildThrowGrenadeIntent(InputManager const &input, Math::Vector2 const &velocity) const
{
    ThrowGrenadeIntent intent{};

    if (input.IsActionPressed(InputAction::ThrowGrenade))
    {
        intent.requested = true;
        intent.origin = GetCenter();
        intent.distance = 200.f;

        if (velocity.x != 0 || velocity.y != 0)
        {
            intent.direction = velocity;
        }
        else
        {
            intent.direction = GetFacingVector();
        }
    }

    return intent;
}

ThrowBombIntent Player::BuildThrowBombIntent(InputManager const &input) const
{
    ThrowBombIntent intent{};
    if (input.IsActionPressed(InputAction::ThrowGrenade))
    {
        intent.requested = true;
        intent.origin = GetCenter();
    }

    return intent;
}

void Player::ApplyMovement(Math::Rect const &bounds)
{
    transform.position = bounds.position;

    transform.position.x = Math::Clamp(transform.position.x, 0.0f, area_size.x - transform.size.x);
    transform.position.y = Math::Clamp(transform.position.y, 0.0f, area_size.y - transform.size.y);
}

void Player::ApplyMovementIntent(MovementIntent const &movement_intent, double frame_delta)
{
    ApplyMovement(movement_intent.next_bounds);

    bool const is_moving = movement_intent.is_moving;
    if (is_moving)
    {
        facing = FacingFromVelocity(movement_intent.velocity);
    }
    animation_indices = GetSpriteIndexByDirection(facing);

    sprite_animation.Update(frame_delta, is_moving);

    sprite_rect = GetSpriteRectByIndex(animation_indices.at(sprite_animation.GetCurrentFrameIndex()));
}

void Player::Initialize(TextureResource const* new_texture_resource, Math::Vector2 const &world_size)
{
    SetDebugName(Config::Player::DebugName);
    SetCollisionEnabled(true);

    sprite_width = Config::Player::SpriteWidth;
    sprite_height = Config::Player::SpriteHeight;

    player_start_point = Config::Player::StartPoint;

    texture_resource = new_texture_resource;
    move_speed = Config::Player::MoveSpeed;
    animation_rate = Config::Player::AnimationRate;
    area_size = world_size;

    animation_indices = GetSpriteIndexByDirection(facing);
    sprite_animation.Initialize(Config::Player::AnimationFrameCount, animation_rate);
    sprite_rect = GetSpriteRectByIndex(animation_indices.at(sprite_animation.GetCurrentFrameIndex()));

    transform.size.x = sprite_width;
    transform.size.y = sprite_height;
    transform.position.x = player_start_point.x;
    transform.position.y = player_start_point.y;
}

void Player::Update(double /*frame_delta*/)
{

}

void Player::Render(SDL_Renderer* renderer, Math::Camera2D const& camera) const
{
    assert(texture_resource != nullptr);
    Math::Vector2 const screen_position = Math::WorldToScreen(transform.position, camera);
    SDL_FRect const player_dst_rect{screen_position.x, screen_position.y, transform.size.x, transform.size.y};
    SDL_RenderTexture(renderer, texture_resource->GetTexture(), &sprite_rect, &player_dst_rect);
}

bool Player::CanCheckTrigger() const
{
    return false;
}

Math::Vector2 Player::GetFacingVector() const
{
    switch (facing)
    {
    case Direction::Up:
        return {0, -1};
    case Direction::Down:
        return {0, 1};
    case Direction::Left:
        return {-1, 0};
    case Direction::Right:
        return {1, 0};
    default:
        return {0, 0};
    }
}