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

class Player;
class Collectible;
class Door;

class IScene
{
public:
    virtual ~IScene() = default;

    virtual bool Initialize(SDL_Renderer* renderer, Math::Vector2 const &window_size) = 0;
    virtual void Update(double frame_delta, InputManager const& input) = 0;
    virtual void Render(SDL_Renderer* renderer) const = 0;
    virtual void Shutdown() = 0;
    virtual void DrawDebugUi() {}
};

class GameplayScene : public IScene
{
    TextureResource texture_resource;
    DebugWorldMarker debug_world_marker{};
    DebugWorldGrid debug_world_grid{};

    bool show_debug_overlay = true;
    bool enable_collision_logs = Config::Debug::EnableCollisionLogs;
    bool enable_collectible_logs = Config::Debug::EnableCollectibleLogs;
    bool enable_trigger_logs = Config::Debug::EnableTriggerLogs;
    bool show_debug_bounds = Config::Debug::ShowDebugBounds;
    bool show_world_grid = Config::Debug::ShowWorldGrid;

    std::vector<std::unique_ptr<Entity>> entities{};
    Player *local_player = nullptr;

    Math::Camera2D camera{};

    Math::Vector2 world_size{};

    int collected_count{};
    bool all_collectibles_collected_logged{};

    void LogSceneError(char const* action) const;
    void LogCollisionBlocker(Entity const &entity) const;
    void LogCollectibleCollected(Entity const &entity) const;
    void LogCollectedTotal() const;
    void LogAllCollectiblesCollected() const;
    void LogEntityMessage(char const *category, char const *action, Entity const &entity) const;
    void LogTriggerActivated(Entity const &entity) const;
    void LogDoorLocked(Door const &door) const;

    template<typename T, typename... Args>
    T *CreateEntity(Args &&...args)
    {
        std::unique_ptr<T> entity = std::make_unique<T>(std::forward<Args>(args)...);
        T *entity_ptr = entity.get();
        entities.push_back(std::move(entity));
        return entity_ptr;
    }

    bool IsAreaFree(Math::Rect const &bounds, Entity const *ignored_entity) const;
    Math::Rect ResolveMovement(Entity const &moving_entity, Math::Rect const &next_bounds_x, Math::Rect const &next_bounds_y) const;
    void RenderDebugBounds(SDL_Renderer* renderer) const;
    void CheckTriggers();
    void CheckCollectibleObjective();
    void HandleCollectibleTrigger(Collectible &collectible);
    void HandleDoorTrigger(Door &door);
    void HandleGenericTrigger(Entity &entity);
    bool IsCollectibleObjectiveComplete() const;

public:
    bool Initialize(SDL_Renderer* renderer, Math::Vector2 const &window_size) override;
    void Update(double frame_delta, InputManager const& input) override;
    void Render(SDL_Renderer* renderer) const override;
    void Shutdown() override;

    int GetCollectedCount() const;
};
