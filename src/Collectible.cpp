#include "Collectible.h"

Collectible::Collectible(Math::Transform const &transform, SDL_Color const &color, SDL_Color const &debug_bounds_color, char const *debug_name)
	: Entity{transform, debug_name}, color{color}, debug_bounds_color{debug_bounds_color}
{
	SetTriggerEnabled(true);
}

void Collectible::Update(double /*frame_delta*/)
{

}

void Collectible::Render(SDL_Renderer* renderer, Math::Camera2D const& camera) const
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	Math::Vector2 screen_position = Math::WorldToScreen(transform.position, camera);
	SDL_FRect rect{screen_position.x, screen_position.y, transform.size.x, transform.size.y};
	SDL_RenderFillRect(renderer, &rect);
}

SDL_Color const &Collectible::GetDebugBoundsColor() const
{
	return debug_bounds_color;
}

bool Collectible::IsCollected() const
{
	return collected;
}

void Collectible::Collect()
{
	if (collected)
	{
		return;
	}

	collected = true;
	SetActive(false);
}

bool Collectible::CanRender() const
{
	return Entity::CanRender() && !collected;
}

bool Collectible::CanRenderDebugBounds() const
{
	return Entity::CanRenderDebugBounds() && !collected;
}

bool Collectible::CanCheckCollision() const
{
	return false;
}

bool Collectible::CanCheckTrigger() const
{
	return Entity::CanCheckTrigger() && !collected;
}

