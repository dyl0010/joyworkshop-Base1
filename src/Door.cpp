#include "Door.h"

Door::Door(Math::Transform const &transform, SDL_Color const &color, char const *debug_name)
    : Entity{transform, debug_name}, color{color}
{
    SetTriggerEnabled(true);
}

void Door::Update(double /*frame_delta*/)
{

}

void Door::Render(SDL_Renderer* renderer, Math::Camera2D const& camera) const
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    Math::Vector2 screen_position = Math::WorldToScreen(transform.position, camera);
    SDL_FRect rect{screen_position.x, screen_position.y, transform.size.x, transform.size.y};
    SDL_RenderFillRect(renderer, &rect);
}

bool Door::CanCheckCollision() const
{
    return false;
}

bool Door::CanCheckTrigger() const
{
    return Entity::CanCheckTrigger() && !activated;
}

void Door::Activate()
{
    if (activated)
    {
        return;
    }

    activated = true;
    SetActive(false);
}

bool Door::IsActivated() const
{
    return activated;
}

bool Door::HasLoggedLocked() const
{
    return locked_logged;
}

void Door::MarkLockedLogged()
{
    if (locked_logged)
    {
        return;
    }

    locked_logged = true;
}