#include "GrenadeTestScene.h"

#include <algorithm>

#include <imgui.h>

#include "Player.h"
#include "Grenade.h"

namespace
{

Math::Vector2 CenterToTopLeft(Math::Vector2 const &center, Math::Vector2 const &size)
{
    return Math::Vector2{
        center.x - size.x * 0.5f,
        center.y - size.y * 0.5f,
    };
}

}  // namespace

void GrenadeTestScene::LogSceneError(char const* action) const
{
    std::fprintf(stderr, "[GrenadeTestScene] %s failed: %s\n", action, SDL_GetError());
}

bool GrenadeTestScene::IsAreaFree(Math::Rect const &bounds, Entity const *ignored_entity) const
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
            return false;
        }
    }
    return true;
}

Math::Rect GrenadeTestScene::ResolveMovement(Entity const &moving_entity, Math::Rect const &next_bounds_x, Math::Rect const &next_bounds_y) const
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

bool GrenadeTestScene::Initialize(SDL_Renderer* renderer, Math::Vector2 const &window_size)
{
    if (!texture_resource.LoadTextureBMP(Config::Assets::PlayerSpriteSheetPath, renderer))
    {
        LogSceneError("LoadTextureBMP");
        Shutdown();
        return false;
    }

    world_size = {Config::World::Width, Config::World::Height};

    debug_world_grid.SetWorldSize(world_size);

    local_player = CreateEntity<Player>();
    local_player->Initialize(&texture_resource, world_size);

    camera.position = {0.0f, 0.0f};
    camera.viewport_size = {static_cast<float>(window_size.x), static_cast<float>(window_size.y)};
    return true;
}

void GrenadeTestScene::Update(double frame_delta, InputManager const& input)
{
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

        // 投掷手雷
        ThrowGrenadeIntent const throw_grenade_intent = local_player->BuildThrowGrenadeIntent(input, movement_intent.velocity);
        if (throw_grenade_intent.requested)
        {
            Math::Vector2 const grenade_size{32.f, 32.f};
            Math::Vector2 const start_position = CenterToTopLeft(throw_grenade_intent.origin, grenade_size);
            Math::Vector2 const target_center{
                throw_grenade_intent.origin.x + throw_grenade_intent.distance * throw_grenade_intent.direction.x,
                throw_grenade_intent.origin.y + throw_grenade_intent.distance * throw_grenade_intent.direction.y
            };
            Math::Vector2 const target_position = CenterToTopLeft(target_center, grenade_size);

            CreateEntity<Grenade>(Math::Transform{start_position, grenade_size}, target_position, Config::PresetColor::White, Config::PresetColor::Blue, "Grenade2");
        }
    }

    entities.erase(
        std::remove_if(
            entities.begin(),
            entities.end(),
            [](std::unique_ptr<Entity> const &entity) {
                if (Grenade const *grenade = dynamic_cast<Grenade const *>(entity.get()))
                {
                    return grenade->IsFinished();
                }
                return false;
            }
        ),
        entities.end()
    );
}

void GrenadeTestScene::Render(SDL_Renderer* renderer) const
{
	debug_world_grid.Render(renderer, camera);

    for (std::unique_ptr<Entity> const &entity: entities)
    {
        if (!entity->CanRender())
        {
            continue;
        }
        entity->Render(renderer, camera);
    }
}

void GrenadeTestScene::Shutdown()
{
    entities.clear();
    local_player = nullptr;
    texture_resource.Reset();
}

void GrenadeTestScene::DrawDebugUi()
{
    std::size_t const grenade_count = std::count_if(
        entities.cbegin(),
        entities.cend(),
        [](std::unique_ptr<Entity> const &entity) {
            return dynamic_cast<Grenade const *>(entity.get());
        }
    );

    ImGui::Begin("Grenade Test");
    ImGui::Text("Grenades: %d", static_cast<int>(grenade_count));
    if (local_player != nullptr)
    {
        ImGui::Text("Player: %.1f, %.1f", local_player->GetPosition().x, local_player->GetPosition().y);
    }
    else
    {
        ImGui::Text("Player: <none>");
    }
    ImGui::Text("Space: throw grenade");
    ImGui::End();
}