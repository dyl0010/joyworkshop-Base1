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
#include "Bomb.h"

class Player;
struct ThrowBombIntent;

class BombTestScene : public IScene
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

    bool player_hit_by_bomb{};

    bool HasActiveBombAt(Math::Vector2 const &tile_position) const;
    void TryPlaceBomb(ThrowBombIntent const &throw_bomb_intent);
    void CheckPlayerBombHit();
    void RenderPlayerHitFeedback(SDL_Renderer* renderer) const;
    void ResetTestState();

    bool IsSolidBlockAt(Math::Rect const &cell_bounds) const;
    bool IsBreakableBlockAt(Math::Rect const &cell_bounds) const;
    std::vector<Math::Rect> GetEffectiveExplosionBounds(Bomb const &bomb) const;
    void RenderBomb(SDL_Renderer* renderer, Bomb const &bomb) const;
    void ApplyBombDamageToBreakableBlocks();

    Math::Vector2 window_size{};

public:
    bool Initialize(SDL_Renderer* renderer, Math::Vector2 const &new_window_size) override;
    void Update(double frame_delta, InputManager const& input) override;
    void Render(SDL_Renderer* renderer) const override;
    void Shutdown() override;
    void DrawDebugUi() override;
};
