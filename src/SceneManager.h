#pragma once

#include <SDL3/SDL.h>

#include <memory>

#include "InputManager.h"
#include "Math.h"

enum class SceneId
{
    Gameplay,
    GrenadeTest,
    BombTest,
};

class IScene;

class SceneManager
{
    std::unique_ptr<IScene> main_scene;
    SceneId current_scene_id{SceneId::Gameplay};
    SceneId pending_scene_id{SceneId::Gameplay};
    bool has_pending_scene_changed{false};

    Math::Vector2 window_size{};

public:
    SceneManager();
    ~SceneManager();

    SceneManager(SceneManager const&) = delete;
    SceneManager& operator=(SceneManager const&) = delete;

    SceneManager(SceneManager&&) = delete;
    SceneManager& operator=(SceneManager&&) = delete;

    bool Initialize(SDL_Renderer* renderer, Math::Vector2 const &window_size);
    void Update(double frame_delta, InputManager const& input);
    void Render(SDL_Renderer* renderer) const;
    void Shutdown();


    SceneId GetCurrentSceneId() const;
    char const *GetCurrentSceneName() const;
    void RequestSceneChange(SceneId scene_id);
    void ProcessPendingSceneChange(SDL_Renderer *renderer, Math::Vector2 const &window_size);

    void DrawCurrentSceneDebugUi();
};
