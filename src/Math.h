#pragma once

namespace Math
{

struct Vector2
{
    float x;
    float y;
};

struct Transform
{
    Vector2 position;
    Vector2 size;
};

struct Camera2D
{
    Vector2 position;
    Vector2 viewport_size;
};

struct Rect
{
    Vector2 position;
    Vector2 size;
};

void Normalize(Vector2& direction);

float Length(Vector2 const& direction);

float Clamp(float value, float min, float max);

float ClampCameraAxis(float desired_position, float world_size, float viewport_size);

Vector2 WorldToScreen(Vector2 const& world, Camera2D const& camera);

void FollowTarget(Camera2D &camera, Vector2 const &center, Vector2 const &world_size);

bool Intersects(Rect const &a, Rect const &b);

} // namespace Math
