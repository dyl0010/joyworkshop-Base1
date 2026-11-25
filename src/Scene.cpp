#include "Scene.h"

#include <cstdio>

#include "Player.h"
#include "Block.h"
#include "Collectible.h"
#include "Door.h"
#include "Movement.h"

void GameplayScene::LogEntityMessage(char const *category, char const *action, Entity const &entity) const
{
    char const *entity_debug_name = entity.GetDebugName();
    if (entity_debug_name != nullptr)
    {
        std::fprintf(stderr, "[%s] %s %s\n", category, action, entity_debug_name);
    }
    else
    {
        std::fprintf(stderr, "[%s] %s entity\n", category, action);
    }
}

void GameplayScene::LogTriggerActivated(Entity const &entity) const
{
    if (!enable_trigger_logs)
    {
        return;
    }

    LogEntityMessage("trigger", "activated", entity);
}

void GameplayScene::LogDoorLocked(Door const &door) const
{
    if (!enable_trigger_logs)
    {
        return;
    }

    LogEntityMessage("door", "locked", door);
}

void GameplayScene::LogCollisionBlocker(Entity const &entity) const
{
    if (!enable_collision_logs)
    {
        return;
    }

    LogEntityMessage("collision", "blocked by", entity);
}

void GameplayScene::LogCollectibleCollected(Entity const &entity) const
{
    if (!enable_collectible_logs)
    {
        return;
    }

    LogEntityMessage("collectible", "collected", entity);
}

void GameplayScene::LogCollectedTotal() const
{
    if (!enable_collectible_logs)
    {
        return;
    }

    std::fprintf(stderr, "[collectible] total collected: %d\n", collected_count);
}

void GameplayScene::LogAllCollectiblesCollected() const
{
    if (!enable_collectible_logs)
    {
        return;
    }

    std::fprintf(stderr, "[collectible] all collected\n");
}

bool GameplayScene::IsAreaFree(Math::Rect const &bounds, Entity const *ignored_entity) const
{
    for (std::unique_ptr<Entity> const &entity: entities)
    {
        if (entity.get() == ignored_entity)
        {
            continue;
        }
        if (!entity->CanCheckCollision())
        {
            continue;
        }
        if (Math::Intersects(entity->GetBounds(), bounds))
        {
            LogCollisionBlocker(*entity);
            return false;
        }
    }
    return true;
}

void GameplayScene::CheckTriggers()
{
    if (local_player == nullptr)
    {
        return;
    }

    for (std::unique_ptr<Entity> const &entity: entities)
    {
        if (entity.get() == local_player)
        {
            continue;
        }
        if (!entity->CanCheckTrigger())
        {
            continue;
        }
        if (!Math::Intersects(entity->GetBounds(), local_player->GetBounds()))
        {
            continue;
        }

        if (Collectible *collectible = dynamic_cast<Collectible*>(entity.get()))
        {
            HandleCollectibleTrigger(*collectible);
        }
        else if (Door *door = dynamic_cast<Door*>(entity.get()))
        {
            HandleDoorTrigger(*door);
        }
        else
        {
            HandleGenericTrigger(*entity);
        }
    }
}

void GameplayScene::CheckCollectibleObjective()
{
    if (IsCollectibleObjectiveComplete() && !all_collectibles_collected_logged)
    {
        all_collectibles_collected_logged = true;
        LogAllCollectiblesCollected();
    }
}

void GameplayScene::HandleDoorTrigger(Door &door)
{
    if (!IsCollectibleObjectiveComplete())
    {
        if (!door.HasLoggedLocked())
        {
            LogDoorLocked(door);
            door.MarkLockedLogged();
        }
        return;
    }

    LogTriggerActivated(door);
    door.Activate();
}

void GameplayScene::HandleCollectibleTrigger(Collectible &collectible)
{
    if (collectible.IsCollected())
    {
        return;
    }
    collectible.Collect();
    ++collected_count;
    LogCollectibleCollected(collectible);
    LogCollectedTotal();
    CheckCollectibleObjective();
}

void GameplayScene::HandleGenericTrigger(Entity &entity)
{
    LogTriggerActivated(entity);
}

bool GameplayScene::IsCollectibleObjectiveComplete() const
{
    return collected_count >= Config::Collectibles::RequiredCount;
}

Math::Rect GameplayScene::ResolveMovement(Entity const &moving_entity, Math::Rect const &next_bounds_x, Math::Rect const &next_bounds_y) const
{
    Math::Rect resolved_bounds = moving_entity.GetBounds();

    if (IsAreaFree(next_bounds_x, &moving_entity))
    {
        resolved_bounds.position.x = next_bounds_x.position.x;
    }

    Math::Rect const y_test_bounds{
        {
            resolved_bounds.position.x,
            next_bounds_y.position.y
        },
        resolved_bounds.size
    };

    if (IsAreaFree(y_test_bounds, &moving_entity))
    {
        resolved_bounds.position.y = next_bounds_y.position.y;
    }

    return resolved_bounds;
}

bool GameplayScene::Initialize(SDL_Renderer* renderer, Math::Vector2 const &window_size)
{
    if (!texture_resource.LoadTextureBMP(Config::Assets::PlayerSpriteSheetPath, renderer))
    {
        LogSceneError("LoadTextureBMP");
        Shutdown();
        return false;
    }

    world_size = {Config::World::Width, Config::World::Height};

    debug_world_grid.SetWorldSize(world_size);

    CreateEntity<Block>(Config::Blocks::PurpleBlockTransform, Config::Blocks::PurpleBlockColor, Config::Blocks::GreenBlockDebugName);
    CreateEntity<Block>(Config::Blocks::BlueBlockTransform, Config::Blocks::BlueBlockColor, Config::Blocks::BlueBlockDebugName);
    CreateEntity<Block>(Config::Blocks::YellowBlockTransform, Config::Blocks::YellowBlockColor, Config::Blocks::YellowBlockDebugName);
    CreateEntity<Block>(Config::Blocks::RedBlockTransform, Config::Blocks::RedBlockColor, Config::Blocks::RedBlockDebugName);
    
    CreateEntity<Collectible>(Config::Collectibles::CoinTransform, Config::Collectibles::CoinColor, Config::Collectibles::CoinBoundsColor, Config::Collectibles::CoinDebugName);
    CreateEntity<Collectible>(Config::Collectibles::GemTransform, Config::Collectibles::GemColor, Config::Collectibles::GemBoundsColor, Config::Collectibles::GemDebugName);

    CreateEntity<Door>(Config::Doors::ExitDoorTransform, Config::Doors::ExitDoorColor, Config::Doors::ExitDoorDebugName);

    local_player = CreateEntity<Player>();
    local_player->Initialize(&texture_resource, world_size);

    camera.position = {0.0f, 0.0f};
    camera.viewport_size = {static_cast<float>(window_size.x), static_cast<float>(window_size.y)};
    return true;
}

void GameplayScene::Update(double frame_delta, InputManager const& input)
{
    if (input.IsActionPressed(InputAction::ToggleDebugOverlay))
    {
        show_debug_overlay = !show_debug_overlay;
    }

    if (input.IsActionPressed(InputAction::ToggleCollisionLogs))
    {
        enable_collision_logs = !enable_collision_logs;
    }

    if (input.IsActionPressed(InputAction::ToggleDebugBounds))
    {
        show_debug_bounds = !show_debug_bounds;
    }

    if (input.IsActionPressed(InputAction::ToggleWorldGrid))
    {
        show_world_grid = !show_world_grid;
    }

    for (std::unique_ptr<Entity> const &entity: entities)
    {
        if (local_player != entity.get())
        {
            entity->Update(frame_delta);
        }
    }

    if (local_player != nullptr)
    {
        MovementIntent movement_intent = local_player->BuildMovementIntent(frame_delta, input);

        movement_intent.next_bounds = ResolveMovement(*local_player, movement_intent.next_bounds_x, movement_intent.next_bounds_y);
        
        local_player->ApplyMovementIntent(movement_intent, frame_delta);

        Math::FollowTarget(camera, local_player->GetCenter(), world_size);

        CheckTriggers();
    }
}

void GameplayScene::RenderDebugBounds(SDL_Renderer* renderer) const
{
    if (!show_debug_bounds || !show_debug_overlay)
    {
        return;
    }

    for (std::unique_ptr<Entity> const &entity: entities)
    {
        if (!entity->CanRenderDebugBounds())
        {
            continue;
        }

        if (entity->IsCollisionEnabled())
        {
            RenderWorldRectOutline(renderer, entity->GetBounds(), camera, Config::Debug::CollisionBoundsColor);
        }
        if (entity->IsTriggerEnabled())
        {
            if (Collectible const *collectible = dynamic_cast<Collectible const *>(entity.get()))
            {
                RenderWorldRectOutline(renderer, entity->GetBounds(), camera, collectible->GetDebugBoundsColor());
            }
            else
            {
                RenderWorldRectOutline(renderer, entity->GetBounds(), camera, Config::Debug::TriggerBoundsColor);
            }
        }
    }
}

void GameplayScene::Render(SDL_Renderer* renderer) const
{
    if (show_world_grid && show_debug_overlay)
    {
        debug_world_grid.Render(renderer, camera);
    }
    if (Config::Debug::ShowWorldMarker && show_debug_overlay)
    {
        debug_world_marker.Render(renderer, camera);
    }

    for (std::unique_ptr<Entity> const &entity: entities)
    {
        if (!entity->CanRender())
        {
            continue;
        }
        entity->Render(renderer, camera);
    }

    RenderDebugBounds(renderer);
}

void GameplayScene::Shutdown()
{
    entities.clear();
    local_player = nullptr;
    texture_resource.Reset();
}

void GameplayScene::LogSceneError(char const* action) const
{
    std::fprintf(stderr, "[scene] %s failed: %s\n", action, SDL_GetError());
}

int GameplayScene::GetCollectedCount() const
{
    return collected_count;
}
