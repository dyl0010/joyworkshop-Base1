#include "DebugUi.h"

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

#include "SceneManager.h"

bool DebugUi::Initialize(SDL_Window *window, SDL_Renderer *renderer)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    initialized = true;
    return initialized;
}

void DebugUi::ProcessEvent(SDL_Event const &event)
{
    if (!initialized)
    {
        return;
    }

    ImGui_ImplSDL3_ProcessEvent(&event);
}

void DebugUi::BeginFrame()
{
    if (!initialized)
    {
        return;
    }

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void DebugUi::Render(SDL_Renderer *renderer, SceneManager &scene_manager)
{
    if (!initialized)
    {
        return;
    }

    ImGui::Begin("Prototype Scenes");
    ImGui::Text("Current: %s", scene_manager.GetCurrentSceneName());
    ImGui::Separator();
    
    if (ImGui::Button("Gameplay"))
    {
        scene_manager.RequestSceneChange(SceneId::Gameplay);
    }
    
    if (ImGui::Button("Grenade Test"))
    {
        scene_manager.RequestSceneChange(SceneId::GrenadeTest);
    }

    if (ImGui::Button("Bomb Test"))
    {
        scene_manager.RequestSceneChange(SceneId::BombTest);
    }

    ImGui::BeginDisabled();
    ImGui::Button("Tilemap Test");
    ImGui::Button("Particle Test");
    ImGui::EndDisabled();
    ImGui::End();

    scene_manager.DrawCurrentSceneDebugUi();

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}

void DebugUi::Shutdown()
{
    if (!initialized)
    {
        return;
    }

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    initialized = false;
}

bool DebugUi::IsInitialize() const
{
    return initialized;
}

bool DebugUi::WantsKeyboardCapture() const
{
    return initialized && ImGui::GetIO().WantCaptureKeyboard;
}

bool DebugUi::WantsMouseCapture() const
{
    return initialized && ImGui::GetIO().WantCaptureMouse;
}
