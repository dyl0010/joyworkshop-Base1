#include "Entity.h"

Entity::Entity(Math::Transform const &transform)
	: transform{transform} {}

Entity::Entity(Math::Transform const &transform, char const *debug_name)
	: transform{transform}, debug_name{debug_name} {}

char const *Entity::GetDebugName() const
{
	return debug_name;
}

void Entity::SetDebugName(char const *new_name)
{
	debug_name = new_name;
}

bool Entity::CanRender() const
{
	return IsActive();
}

bool Entity::CanRenderDebugBounds() const
{
	return IsActive() && (IsCollisionEnabled() || IsTriggerEnabled());
}

bool Entity::CanCheckCollision() const
{
	return IsActive() && IsCollisionEnabled();
}

bool Entity::CanCheckTrigger() const
{
	return IsActive() && IsTriggerEnabled();
}
