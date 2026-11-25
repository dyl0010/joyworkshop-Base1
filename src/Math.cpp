#include "Math.h"

#include <cassert>
#include <cmath>

namespace Math
{

void Normalize(Vector2& direction)
{
    if (direction.x == 0.0f && direction.y == 0.0f)
    {
        return;
    }

    Vector2 const original_direction{direction};
    float const length = std::sqrt(
        original_direction.x * original_direction.x + original_direction.y * original_direction.y);
    direction.x = original_direction.x / length;
    direction.y = original_direction.y / length;
}

float Length(Vector2 const& direction)
{
    return std::sqrt(direction.x * direction.x + direction.y * direction.y);
}

float Clamp(float value, float min, float max)
{
    assert(min <= max);
    if (value < min)
    {
        return min;
    }
    if (value > max)
    {
        return max;
    }

    return value;
}

float ClampCameraAxis(float desired_position, float world_size, float viewport_size)
{
    if (world_size <= viewport_size)
        return 0.f;
    return Clamp(desired_position, 0.f, world_size - viewport_size);
}

Vector2 WorldToScreen(Vector2 const& world, Camera2D const& camera)
{
    return Vector2{world.x - camera.position.x, world.y - camera.position.y};
}

void FollowTarget(Camera2D &camera, Vector2 const &center, Vector2 const &world_size)
{
    camera.position.x = center.x - camera.viewport_size.x / 2.f;
    camera.position.y = center.y - camera.viewport_size.y / 2.f;
    camera.position.x = ClampCameraAxis(camera.position.x, world_size.x, camera.viewport_size.x);
    camera.position.y = ClampCameraAxis(camera.position.y, world_size.y, camera.viewport_size.y);
}

bool Intersects(Rect const &a, Rect const &b)
{
    return a.position.x < b.position.x + b.size.x
        && a.position.x + a.size.x > b.position.x
        && a.position.y < b.position.y + b.size.y
        && a.position.y + a.size.y > b.position.y;
}

} // namespace Math
