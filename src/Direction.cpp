#include "Direction.h"

#include <cmath>

Direction FacingFromVelocity(Math::Vector2 const &velocity)
{
	if (std::abs(velocity.x) >= std::abs(velocity.y))
    {
        if (velocity.x > 0.f)
        {
            return Direction::Right;
        }

        return Direction::Left;
    }

    if (velocity.y > 0.f)
    {
        return Direction::Down;
    }

    return Direction::Up;
}