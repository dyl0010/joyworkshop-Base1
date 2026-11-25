#include "BreakableBlock.h"

BreakableBlock::BreakableBlock(Math::Transform const &transform, SDL_Color const &color, char const *debug_name)
    : Entity{transform, debug_name}, color{color}
{
    SetCollisionEnabled(true);
}

void BreakableBlock::Update(double /*frame_delta*/)
{

}

void BreakableBlock::Render(SDL_Renderer* renderer, Math::Camera2D const& camera) const
{
    if (destroyed)
    {
        return;
    }

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    Math::Vector2 screen_position = Math::WorldToScreen(transform.position, camera);
    SDL_FRect rect{screen_position.x, screen_position.y, transform.size.x, transform.size.y};
    SDL_RenderFillRect(renderer, &rect);
}

bool BreakableBlock::CanCheckTrigger() const
{
    return false;
}

void BreakableBlock::Destroy()
{
    if (destroyed)
    {
        return;
    }

    destroyed = true;
    SetCollisionEnabled(false);
    SetActive(false);
}

bool BreakableBlock::IsDestroyed() const
{
    return destroyed;
}

