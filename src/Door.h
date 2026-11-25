#pragma once

#include "Entity.h"

class Door : public Entity
{
    SDL_Color color{};

    bool activated{false};
    bool locked_logged{false};
    
public:
    Door(Math::Transform const &transform, SDL_Color const &color, char const *debug_name);
    void Update(double frame_delta) override;
    void Render(SDL_Renderer* renderer, Math::Camera2D const& camera) const override;

    bool CanCheckCollision() const override;
    bool CanCheckTrigger() const override;

    // Door activation is one-shot; activated doors become inactive.
    void Activate();
    bool IsActivated() const;

    bool HasLoggedLocked() const;
    void MarkLockedLogged();
};