#pragma once

#include <SDL3/SDL.h>

class SceneManager;

class DebugUi
{
    bool initialized{false};

public:
    bool Initialize(SDL_Window *window, SDL_Renderer *renderer);
    void ProcessEvent(SDL_Event const &event);
    void BeginFrame();
    void Render(SDL_Renderer *renderer, SceneManager &scene_manager);
    void Shutdown();

    bool IsInitialize() const;

    bool WantsKeyboardCapture() const;
    bool WantsMouseCapture() const;
};