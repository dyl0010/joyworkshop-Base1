#include "Game.h"

#include <algorithm>
#include <cstdio>

#include "Config.h"

Game::~Game()
{
	Shutdown();
}

bool Game::Initialize(int width, int height, std::string const& title)
{
    window_size = {static_cast<float>(width), static_cast<float>(height)};
    window_title = title;

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        LogSDLError("SDL_Init");
        return false;
    }

    sdl_initialized = true;

    window = SDL_CreateWindow(window_title.c_str(), width, height, SDL_WINDOW_RESIZABLE);
    if (window == nullptr)
    {
        LogSDLError("SDL_CreateWindow");
        Shutdown();
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr)
    {
        LogSDLError("SDL_CreateRenderer");
        Shutdown();
        return false;
    }

    if (!debug_ui.Initialize(window, renderer))
    {
        LogSDLError("debug_ui.Initialize");
        Shutdown();
        return false;
    }

    bool const initialized_scene = scene_manager.Initialize(renderer, window_size);
    if (!initialized_scene)
    {
        Shutdown();
    }

    return initialized_scene;
}

void Game::RunLoop()
{
    Uint64 last_counter = SDL_GetPerformanceCounter();

    constexpr double FixedDelta = Config::Timing::FixedDelta;
    double accumulator = 0.0;

    while (running)
    {
        Uint64 const current_counter = SDL_GetPerformanceCounter();
        double frame_delta = static_cast<double>(current_counter - last_counter) /
            static_cast<double>(SDL_GetPerformanceFrequency());
        last_counter = current_counter;

        while (SDL_PollEvent(&event))
        {
            debug_ui.ProcessEvent(event);

            bool const is_quit_event = event.type == SDL_EVENT_QUIT;
            bool const is_escape_key_down = event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE;
            bool const is_keyboard_event = input_manager.IsKeyboardEvent(event);
            bool const is_mouse_event = input_manager.IsMouseEvent(event);

            if (is_quit_event ||
                is_escape_key_down ||
                (is_keyboard_event && !debug_ui.WantsKeyboardCapture()) ||
                (is_mouse_event && !debug_ui.WantsMouseCapture()))
            {
                input_manager.HandleInput(event, running);
            }
        }

        frame_delta = std::min(frame_delta, Config::Timing::MaxFrameDelta);
        accumulator += frame_delta;
        bool updated_this_frame = false;
        int fixed_update_count = 0;
        while (accumulator >= FixedDelta)
        {
            if (fixed_update_count >= Config::Timing::MaxFixedUpdatesPerFrame)
            {
                if (Config::Debug::EnableDroppedFrameTimingLogs)
                {
                    std::fprintf(stderr, "[timing] dropped fixed update backlog\n");
                }
                accumulator = 0.0;
                break;
            }

            if (updated_this_frame)
            {
                input_manager.CleanInput();
            }

            Update(FixedDelta, input_manager);
            updated_this_frame = true;
            accumulator -= FixedDelta;
            ++fixed_update_count;
        }

        if (updated_this_frame)
        {
            input_manager.CleanInput();
        }

        if (Config::Debug::EnableFrameTimingLogs)
        {
            std::fprintf(stderr, "[timing] fixed updates this frame: %d\n", fixed_update_count);
        }

        scene_manager.ProcessPendingSceneChange(renderer, window_size);

        RenderFrame();
    }
}

void Game::Update(double frame_delta, InputManager const& input)
{
    scene_manager.Update(frame_delta, input);
}

void Game::RenderFrame()
{
    SDL_Color const bg = Config::PresetColor::Background;
    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(renderer);

    scene_manager.Render(renderer);

    debug_ui.BeginFrame();
    debug_ui.Render(renderer, scene_manager);

    SDL_RenderPresent(renderer);
}

void Game::Shutdown()
{
    scene_manager.Shutdown();

    debug_ui.Shutdown();

    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    if (sdl_initialized)
    {
        SDL_Quit();
        sdl_initialized = false;
    }
}

void Game::LogSDLError(char const* action) const
{
    std::fprintf(stderr, "[sdl] %s failed: %s\n", action, SDL_GetError());
}
