#pragma once

#include "Math.h"

struct MovementIntent
{
    Math::Vector2 velocity{};   // 归一化后的移动方向
    Math::Rect next_bounds{};   // 按当前输入和delta预测出的下一帧bounds
    Math::Rect next_bounds_x{}; // 只应用velocity.x后的bounds
    Math::Rect next_bounds_y{}; // 只应用velocity.y后的bounds
    bool is_moving{};           // 是否有移动输入
};

struct ThrowGrenadeIntent
{
    bool requested{};
    Math::Vector2 origin{};
    Math::Vector2 direction{};
    float distance{};
};

struct ThrowBombIntent
{
    bool requested{};
    Math::Vector2 origin{};
};