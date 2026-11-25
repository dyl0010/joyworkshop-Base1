#include "SceneManager.h"

#include <cassert>
#include <memory>

#include "Scene.h"
#include "GrenadeTestScene.h"
#include "BombTestScene.h"

SceneManager::SceneManager() = default;

SceneManager::~SceneManager()
{
    Shutdown();
}

bool SceneManager::Initialize(SDL_Renderer* renderer, Math::Vector2 const &new_window_size)
{
    main_scene = std::make_unique<GameplayScene>();
    current_scene_id = SceneId::Gameplay;
    window_size = new_window_size;

    bool const initialized_scene = main_scene->Initialize(renderer, window_size);
    if (!initialized_scene)
    {
        Shutdown();
    }

    return initialized_scene;
}

void SceneManager::Update(double frame_delta, InputManager const& input)
{
    assert(main_scene != nullptr);
    main_scene->Update(frame_delta, input);
}

void SceneManager::Render(SDL_Renderer* renderer) const
{
    assert(main_scene != nullptr);
    main_scene->Render(renderer);
}

void SceneManager::Shutdown()
{
    if (main_scene)
    {
        main_scene->Shutdown();
        main_scene.reset();
    }
}

SceneId SceneManager::GetCurrentSceneId() const
{
    return current_scene_id;
}

char const *SceneManager::GetCurrentSceneName() const
{
    if (current_scene_id == SceneId::Gameplay)
    {
        return "Gameplay";
    }
    else if (current_scene_id == SceneId::GrenadeTest)
    {
        return "GrenadeTest";
    }
    else if (current_scene_id ==SceneId::BombTest)
    {
        return "BombTest";
    }

    return "";
}

void SceneManager::RequestSceneChange(SceneId scene_id)
{
    if (current_scene_id == scene_id)
    {
        return;
    }

    pending_scene_id = scene_id;
    has_pending_scene_changed = true;
}

void SceneManager::ProcessPendingSceneChange(SDL_Renderer *renderer, Math::Vector2 const &new_window_size)
{
    window_size = new_window_size;

    if (!has_pending_scene_changed)
    {
        return;
    }

    if (main_scene)
    {
        main_scene->Shutdown();
    }

    if (pending_scene_id == SceneId::Gameplay)
    {
        main_scene = std::make_unique<GameplayScene>();
    }
    else if (pending_scene_id == SceneId::GrenadeTest)
    {
        main_scene = std::make_unique<GrenadeTestScene>();
    }
    else if (pending_scene_id == SceneId::BombTest)
    {
        main_scene = std::make_unique<BombTestScene>();
    }

    current_scene_id = pending_scene_id;
    has_pending_scene_changed = false;

    if (!main_scene->Initialize(renderer, new_window_size))
    {
        Shutdown();
    }
}

void SceneManager::DrawCurrentSceneDebugUi()
{
    if (main_scene)
    {
        main_scene->DrawDebugUi();
    }
}
