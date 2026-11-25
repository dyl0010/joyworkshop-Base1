#include "Block.h"

Block::Block(Math::Transform const &transform, SDL_Color const &color, char const *debug_name)
	: Entity{transform, debug_name}, color{color}
{
	SetCollisionEnabled(true);
}

void Block::Update(double /*frame_delta*/)
{

}

void Block::Render(SDL_Renderer* renderer, Math::Camera2D const& camera) const
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	Math::Vector2 screen_position = Math::WorldToScreen(transform.position, camera);
	SDL_FRect rect{screen_position.x, screen_position.y, transform.size.x, transform.size.y};
	SDL_RenderFillRect(renderer, &rect);
}

bool Block::CanCheckTrigger() const
{
	return false;
}
