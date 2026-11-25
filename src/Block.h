#pragma once

#include <SDL3/SDL.h>

#include "Entity.h"
#include "Math.h"

class Block : public Entity
{
	SDL_Color color{};

public:
	Block(Math::Transform const &transform, SDL_Color const &color, char const *debug_name);
	void Update(double frame_delta) override;
    void Render(SDL_Renderer* renderer, Math::Camera2D const& camera) const override;

    bool CanCheckTrigger() const override;
};
