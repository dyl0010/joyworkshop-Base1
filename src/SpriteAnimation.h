#pragma once

#include <cstddef>

class SpriteAnimation
{
    std::size_t frame_count{};
    double animation_duration{};
    double frame_timer{};
    std::size_t current_frame_index{};

public:
    void Initialize(std::size_t count, double rate);
    void Update(double frame_delta, bool is_moving);

    std::size_t GetCurrentFrameIndex() const;
};