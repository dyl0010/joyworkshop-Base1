#pragma once

#include <SDL3/SDL.h>

#include <string>

#include "InputManager.h"
#include "SceneManager.h"
#include "Math.h"
#include "DebugUi.h"

class Game
{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Event event{};

    InputManager input_manager{};
    SceneManager scene_manager{};

    bool running = true;
    bool sdl_initialized = false;

    Math::Vector2 window_size{};
    std::string window_title{"Untitled"};

    DebugUi debug_ui{};

    void RenderFrame();
    void Update(double frame_delta, InputManager const& input);
    void LogSDLError(char const* action) const;

public:
    Game() = default;
    ~Game();

    Game(Game const&) = delete;
    Game& operator=(Game const&) = delete;

    Game(Game&&) = delete;
    Game& operator=(Game&&) = delete;

    bool Initialize(int width, int height, std::string const& title);
    void RunLoop();
    void Shutdown();
};
