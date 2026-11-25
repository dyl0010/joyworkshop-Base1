#pragma once

#include <SDL3/SDL.h>

#include "Math.h"

class Entity
{
protected:
    Math::Transform transform{};

    bool collision_enabled{false};
    bool trigger_enabled{false};

    char const *debug_name = nullptr;

    bool active{true};

public:
	Entity() = default;
    explicit Entity(Math::Transform const &transform);
	explicit Entity(Math::Transform const &transform, char const *debug_name);
	virtual ~Entity() = default;

    virtual void Update(double frame_delta) = 0;
    virtual void Render(SDL_Renderer* renderer, Math::Camera2D const& camera) const = 0;

    Math::Vector2 const &GetPosition() const {return transform.position;}
    Math::Vector2 const &GetSize() const {return transform.size;}
    Math::Transform const &GetTransform() const {return transform;}
    Math::Rect GetBounds() const {return Math::Rect{transform.position, transform.size};}
    Math::Vector2 GetCenter() const {return Math::Vector2{transform.position.x + transform.size.x/2.f, transform.position.y + transform.size.y/2.f};}
    bool IsCollisionEnabled() const {return collision_enabled;}
    bool IsTriggerEnabled() const {return trigger_enabled;}
    bool IsActive() const {return active;}

    char const *GetDebugName() const;
    void SetDebugName(char const *new_name);

    void SetPosition(Math::Vector2 const &new_position) {transform.position = new_position;}
    void SetSize(Math::Vector2 const &new_size) {transform.size = new_size;}
    void SetCollisionEnabled(bool enabled) {collision_enabled = enabled;}
    void SetTriggerEnabled(bool enabled) {trigger_enabled = enabled;}
    void SetActive(bool enabled) {active = enabled;}

    // Whether the scene should call Render() for this entity.
    virtual bool CanRender() const;

    // Whether debug bounds should be drawn for this entity.
    virtual bool CanRenderDebugBounds() const;

    // Whether this entity should be considered as a collision blocker.
    virtual bool CanCheckCollision() const;

    // Whether this entity should be considered as a trigger source.
    virtual bool CanCheckTrigger() const;
};
