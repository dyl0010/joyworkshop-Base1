#pragma once

#include "Math.h"

enum class Direction
{
    Up,
    Down,
    Left,
    Right,
};

Direction FacingFromVelocity(Math::Vector2 const &velocity);