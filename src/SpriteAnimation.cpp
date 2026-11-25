#include "SpriteAnimation.h"

#include <cassert>

void SpriteAnimation::Initialize(std::size_t count, double rate)
{
    assert(count > 0);
    assert(rate > 0.0);
    frame_count = count;
    animation_duration = rate;
    frame_timer = 0.0;
    current_frame_index = 0;
}

void SpriteAnimation::Update(double frame_delta, bool is_moving)
{
    if (is_moving)
    {
        frame_timer += frame_delta;
        while (frame_timer >= animation_duration)
        {
            ++current_frame_index;
            current_frame_index %= frame_count;
            frame_timer -= animation_duration;
        }
    }
    else
    {
        current_frame_index = 0;
        frame_timer = 0.0;
    }
}

std::size_t SpriteAnimation::GetCurrentFrameIndex() const
{
    return current_frame_index;
}