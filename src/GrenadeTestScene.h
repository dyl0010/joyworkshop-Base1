#pragma once

#include <SDL3/SDL.h>

#include <vector>
#include <memory>
#include <utility>

#include "Config.h"
#include "InputManager.h"
#include "Math.h"
#include "Resource.h"
#include "DebugDraw.h"
#include "Entity.h"
#include "Scene.h"

class Player;

class GrenadeTestScene : public IScene
{
    TextureResource texture_resource;
    DebugWorldGrid debug_world_grid{};

    std::vector<std::unique_ptr<Entity>> entities{};
    Player *local_player = nullptr;

    Math::Camera2D camera{};

    Math::Vector2 world_size{};

    template<typename T, typename... Args>
    T *CreateEntity(Args &&...args)
    {
        std::unique_ptr<T> entity = std::make_unique<T>(std::forward<Args>(args)...);
        T *entity_ptr = entity.get();
        entities.push_back(std::move(entity));
        return entity_ptr;
    }

    void LogSceneError(char const* action) const;

    bool IsAreaFree(Math::Rect const &bounds, Entity const *ignored_entity) const;
    Math::Rect ResolveMovement(Entity const &moving_entity, Math::Rect const &next_bounds_x, Math::Rect const &next_bounds_y) const;

public:
    bool Initialize(SDL_Renderer* renderer, Math::Vector2 const &window_size) override;
    void Update(double frame_delta, InputManager const& input) override;
    void Render(SDL_Renderer* renderer) const override;
    void Shutdown() override;
    void DrawDebugUi() override;
};
