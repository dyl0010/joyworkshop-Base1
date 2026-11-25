#include "BombTestScene.h"

#include <algorithm>
#include <cmath>
#include <array>

#include <imgui.h>

#include "Player.h"
#include "Block.h"
#include "BreakableBlock.h"

namespace
{

Math::Vector2 CenterToTopLeft(Math::Vector2 const &center, Math::Vector2 const &size)
{
    return Math::Vector2{
        center.x - size.x * 0.5f,
        center.y - size.y * 0.5f,
    };
}

constexpr float BombTestTileSize = 64.f;
constexpr SDL_Color BombTestExplosionColor{255, 100, 0, 255};

}  // namespace

void BombTestScene::LogSceneError(char const* action) const
{
    std::fprintf(stderr, "[BombTestScene] %s failed: %s\n", action, SDL_GetError());
}

bool BombTestScene::IsAreaFree(Math::Rect const &bounds, Entity const *ignored_entity) const
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
        if (ignored_entity == local_player)
        {
            if (Bomb const *bomb = dynamic_cast<Bomb const *>(entity.get()))
            {
                if (Math::Intersects(bomb->GetBounds(), local_player->GetBounds()))
                {
                    continue;
                }
            }
        }
        if (Math::Intersects(entity->GetBounds(), bounds))
        {
            return false;
        }
    }
    return true;
}

Math::Rect BombTestScene::ResolveMovement(Entity const &moving_entity, Math::Rect const &next_bounds_x, Math::Rect const &next_bounds_y) const
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

bool BombTestScene::HasActiveBombAt(Math::Vector2 const &tile_position) const
{
    auto found = std::find_if(
        entities.cbegin(),
        entities.cend(),
        [&tile_position](std::unique_ptr<Entity> const &entity) {
            Bomb const *bomb = dynamic_cast<Bomb const *>(entity.get());
            if (bomb == nullptr)
            {
                return false;
            }
            if (bomb->IsFinished())
            {
                return false;
            }
            return bomb->GetPosition().x == tile_position.x && bomb->GetPosition().y == tile_position.y;
        }
    );

    return found != entities.cend();
}

void BombTestScene::TryPlaceBomb(ThrowBombIntent const &throw_bomb_intent)
{
    if (throw_bomb_intent.requested)
    {
        Math::Vector2 const bomb_size{BombTestTileSize, BombTestTileSize};

        int const align_count_x = static_cast<int>(throw_bomb_intent.origin.x / BombTestTileSize);
        int const align_count_y = static_cast<int>(throw_bomb_intent.origin.y / BombTestTileSize);
        Math::Vector2 const align_position{
            align_count_x * BombTestTileSize,
            align_count_y * BombTestTileSize
        };

        // 防止在同一位置重复放置bomb
        if (!HasActiveBombAt(align_position))
        {
            CreateEntity<Bomb>(Math::Transform{align_position, bomb_size}, "Bomb");
        }
    }
}

void BombTestScene::CheckPlayerBombHit()
{
    if (player_hit_by_bomb)
    {
        return;
    }

    if (local_player == nullptr)
    {
        return;
    }

    for (std::unique_ptr<Entity> const &entity: entities)
    {
        if (Bomb const *bomb = dynamic_cast<Bomb const *>(entity.get()))
        {
            std::vector<Math::Rect> explosion_bounds = GetEffectiveExplosionBounds(*bomb);
            auto found = std::find_if(
                explosion_bounds.cbegin(),
                explosion_bounds.cend(),
                [this] (Math::Rect const &rect) {
                    return Math::Intersects(local_player->GetBounds(), rect);
                }
            );
            if (found != explosion_bounds.cend())
            {
                player_hit_by_bomb = true;
                return;
            }
        }
    }
}

void BombTestScene::RenderPlayerHitFeedback(SDL_Renderer* renderer) const
{
    if (!player_hit_by_bomb || local_player == nullptr)
    {
        return;
    }

    SDL_Color color = Config::PresetColor::Red;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    Math::Rect const player_bounds = local_player->GetBounds();
    Math::Vector2 const screen_position = Math::WorldToScreen(player_bounds.position, camera);
    SDL_FRect const hit_box{screen_position.x, screen_position.y, player_bounds.size.x, player_bounds.size.y};
    SDL_RenderRect(renderer, &hit_box);
}

void BombTestScene::ResetTestState()
{
    entities.clear();

    local_player = CreateEntity<Player>();
    local_player->Initialize(&texture_resource, world_size);

    player_hit_by_bomb = false;
    camera.position = {0.f, 0.f};

    camera.viewport_size = {static_cast<float>(window_size.x), static_cast<float>(window_size.y)};

    // 关卡中Block的分布
    std::array<Math::Vector2, 4> block_locations{{
        {5, 4},
        {6, 4},
        {7, 4},
        {5, 5},
    }};
    std::for_each(
        block_locations.cbegin(),
        block_locations.cend(),
        [this] (Math::Vector2 const &location) {
            Math::Transform const transform{
                location.x * BombTestTileSize,
                location.y * BombTestTileSize,
                BombTestTileSize,
                BombTestTileSize
            };
            CreateEntity<Block>(transform, Config::PresetColor::Purple, "SolidBlock");
        }
    );

    // 关卡中的BreakableBlock分布
    std::array<Math::Vector2, 4> breakable_block_locations{{
        {4, 4},
        {4, 5},
        {6, 5},
        {8, 4},
    }};
    std::for_each(
        breakable_block_locations.cbegin(),
        breakable_block_locations.cend(),
        [this] (Math::Vector2 const &location)
        {
            Math::Transform const transform{
                location.x * BombTestTileSize,
                location.y * BombTestTileSize,
                BombTestTileSize,
                BombTestTileSize
            };
            CreateEntity<BreakableBlock>(transform, SDL_Color{160, 100, 45, 255}, "BreakableBlock");
        }
    );
}

bool BombTestScene::IsSolidBlockAt(Math::Rect const &cell_bounds) const
{
    auto found = std::find_if(
        entities.cbegin(),
        entities.cend(),
        [&cell_bounds] (std::unique_ptr<Entity> const &entity) {
            if (!dynamic_cast<Block const *>(entity.get()))
            {
                return false;
            }

            Math::Rect const bounds = entity->GetBounds();
            return bounds.position.x == cell_bounds.position.x &&
                bounds.position.y == cell_bounds.position.y;
        }
    );
    return found != entities.cend();
}

bool BombTestScene::IsBreakableBlockAt(Math::Rect const &cell_bounds) const
{
    auto found = std::find_if(
        entities.cbegin(),
        entities.cend(),
        [&cell_bounds] (std::unique_ptr<Entity> const &entity) {
            auto breakable = dynamic_cast<BreakableBlock const *>(entity.get());
            if (breakable == nullptr || !breakable->IsActive() || breakable->IsDestroyed())
            {
                return false;
            }

            Math::Rect const bounds = entity->GetBounds();
            return bounds.position.x == cell_bounds.position.x &&
                bounds.position.y == cell_bounds.position.y;
        }
    );
    return found != entities.cend();
}

std::vector<Math::Rect> BombTestScene::GetEffectiveExplosionBounds(Bomb const &bomb) const
{
    if (!bomb.HasExploded() || bomb.IsFinished())
    {
        return {};
    }

    int const explosion_range = bomb.GetExplosionRange();
    std::vector<Math::Rect> effective_explosion_bounds{};

    constexpr std::array<std::pair<int, int>, 4> directions{{
        {0, -1},
        {0, 1},
        {-1, 0},
        {1, 0},
    }};

    Math::Rect bomb_bounds = bomb.GetBounds();
    effective_explosion_bounds.push_back(bomb_bounds);
    std::for_each(
        directions.cbegin(),
        directions.cend(),
        [&] (std::pair<int, int> const &direction)
        {
            for (int i = 1; i <= explosion_range; ++i)
            {
                Math::Rect const bounds {
                    {
                        bomb_bounds.position.x + i * direction.first * bomb_bounds.size.x,
                        bomb_bounds.position.y + i * direction.second * bomb_bounds.size.y
                    },
                    bomb_bounds.size
                };
                if (IsSolidBlockAt(bounds))
                {
                    break;
                }
                
                effective_explosion_bounds.push_back(bounds);
                
                if (IsBreakableBlockAt(bounds))
                {
                    break;
                }
            }
        }
    );

    return effective_explosion_bounds;
}

void BombTestScene::RenderBomb(SDL_Renderer* renderer, Bomb const &bomb) const
{
    std::vector<Math::Rect> const effective_explosion_bounds = GetEffectiveExplosionBounds(bomb);
    std::for_each(
        effective_explosion_bounds.cbegin(),
        effective_explosion_bounds.cend(),
        [this, renderer] (Math::Rect const &bounds) {
            Math::Vector2 const screen_position = Math::WorldToScreen(bounds.position, camera);
            SDL_FRect const screen_rect{screen_position.x, screen_position.y, bounds.size.x, bounds.size.y};
            SDL_SetRenderDrawColor(renderer, BombTestExplosionColor.r, BombTestExplosionColor.g, BombTestExplosionColor.b, BombTestExplosionColor.a);
            SDL_RenderRect(renderer, &screen_rect);
        }
    );
}

void BombTestScene::ApplyBombDamageToBreakableBlocks()
{
    for (std::unique_ptr<Entity> const &entity : entities)
    {
        Bomb const *bomb = dynamic_cast<Bomb const *>(entity.get());
        
        if (bomb == nullptr || !bomb->HasExploded() || bomb->IsFinished())
        {
            continue;
        }

        std::vector<Math::Rect> const effective_explosion_bounds = GetEffectiveExplosionBounds(*bomb);
        std::for_each(
            entities.cbegin(),
            entities.cend(),
            [&] (std::unique_ptr<Entity> const &entity)
            {
                if (BreakableBlock *breakable = dynamic_cast<BreakableBlock*>(entity.get()))
                {
                    auto found = std::find_if(
                        effective_explosion_bounds.cbegin(),
                        effective_explosion_bounds.cend(),
                        [&breakable] (Math::Rect const &bounds)
                        {
                            return !breakable->IsDestroyed() && Math::Intersects(breakable->GetBounds(), bounds);
                        }
                    );

                    if (found != effective_explosion_bounds.cend())
                    {
                        breakable->Destroy();
                    }
                }
            }
        );
    }
}

bool BombTestScene::Initialize(SDL_Renderer* renderer, Math::Vector2 const &new_window_size)
{
    if (!texture_resource.LoadTextureBMP(Config::Assets::PlayerSpriteSheetPath, renderer))
    {
        LogSceneError("LoadTextureBMP");
        Shutdown();
        return false;
    }

    world_size = {Config::World::Width, Config::World::Height};

    window_size = new_window_size;

    debug_world_grid.SetWorldSize(world_size);

    ResetTestState();

    return true;
}

void BombTestScene::Update(double frame_delta, InputManager const& input)
{
    for (std::unique_ptr<Entity> const &entity: entities)
    {
        if (local_player != entity.get())
        {
            entity->Update(frame_delta);
        }
    }

    CheckPlayerBombHit();

    ApplyBombDamageToBreakableBlocks();

    if (local_player != nullptr && !player_hit_by_bomb)
    {   
        MovementIntent movement_intent = local_player->BuildMovementIntent(frame_delta, input);

        movement_intent.next_bounds = ResolveMovement(*local_player, movement_intent.next_bounds_x, movement_intent.next_bounds_y);

        local_player->ApplyMovementIntent(movement_intent, frame_delta);

        Math::FollowTarget(camera, local_player->GetCenter(), world_size);

        // 放置bomb
        ThrowBombIntent const throw_bomb_intent = local_player->BuildThrowBombIntent(input);
        TryPlaceBomb(throw_bomb_intent);
    }
    else if (local_player != nullptr)
    {
        Math::FollowTarget(camera, local_player->GetCenter(), world_size);
    }

    entities.erase(
        std::remove_if(
            entities.begin(),
            entities.end(),
            [](std::unique_ptr<Entity> const &entity) {
                if (Bomb const *bomb = dynamic_cast<Bomb const *>(entity.get()))
                {
                    return bomb->IsFinished();
                }
                return false;
            }
        ),
        entities.end()
    );
}

void BombTestScene::Render(SDL_Renderer* renderer) const
{
	debug_world_grid.Render(renderer, camera);

    for (std::unique_ptr<Entity> const &entity: entities)
    {
        if (entity.get() == local_player)
        {
            continue;
        }
        if (!entity->CanRender())
        {
            continue;
        }

        // 在这里临时渲染Bomb的实际爆炸范围
        if (Bomb const *bomb = dynamic_cast<Bomb const *>(entity.get()); bomb != nullptr && bomb->HasExploded())
        {
            RenderBomb(renderer, *bomb);
        }
        else
        {
            entity->Render(renderer, camera);
        }
    }
    // 最后绘制玩家，让玩家显示在最前面
    if (local_player != nullptr && local_player->CanRender())
    {
        local_player->Render(renderer, camera);
    }

    // 玩家受击提示
    RenderPlayerHitFeedback(renderer);
}

void BombTestScene::Shutdown()
{
    entities.clear();
    local_player = nullptr;
    texture_resource.Reset();
}

void BombTestScene::DrawDebugUi()
{
    int bomb_count{};
    int solid_block_count{};
    int breakable_block_count{};
    int active_breakable_block_count{};

    std::for_each(
        entities.cbegin(),
        entities.cend(),
        [&] (std::unique_ptr<Entity> const &entity)
        {
            if (dynamic_cast<Bomb const *>(entity.get()))
            {
                ++bomb_count;
            }
            else if (dynamic_cast<Block const *>(entity.get()))
            {
                ++solid_block_count;
            }
            else if (BreakableBlock const *breakblock = dynamic_cast<BreakableBlock const *>(entity.get()))
            {
                ++breakable_block_count;
                
                if (breakblock->IsActive())
                {
                    ++active_breakable_block_count;
                }
            }
        }
    );

    ImGui::Begin("Bomb Test");
    ImGui::Text("Bombs: %d", bomb_count);
    ImGui::Text("Blocks: %d", solid_block_count);
    ImGui::Text("Breakable blocks: %d / %d", active_breakable_block_count, breakable_block_count);
    if (local_player != nullptr)
    {
        ImGui::Text("Player: %.1f, %.1f", local_player->GetPosition().x, local_player->GetPosition().y);
    }
    else
    {
        ImGui::Text("Player: <none>");
    }
    ImGui::Text("Space: place bomb");
    ImGui::Separator();
    ImGui::Text("Player hit: %s", player_hit_by_bomb ? "yes" : "no");
    if (ImGui::Button("Reset Test"))
    {
        ResetTestState();
    }
    ImGui::End();
}