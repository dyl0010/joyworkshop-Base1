#include "DebugDraw.h"

#include "Config.h"

void RenderWorldRectOutline(
    SDL_Renderer* renderer,
    Math::Rect const &rect,
    Math::Camera2D const &camera,
    SDL_Color const &color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    Math::Vector2 const bounds_screen = Math::WorldToScreen(rect.position, camera);
    SDL_FRect const collision_rect{bounds_screen.x, bounds_screen.y, rect.size.x, rect.size.y};
    SDL_RenderRect(renderer, &collision_rect);
}

void DebugWorldMarker::Render(SDL_Renderer *renderer, Math::Camera2D const &camera) const
{
    Math::Vector2 const screen_position = Math::WorldToScreen(Config::Debug::MarkerStartPoint, camera);
    SDL_Color const &color = Config::Debug::MarkerColor;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_FRect rect{screen_position.x, screen_position.y, Config::Debug::MarkerWidth, Config::Debug::MarkerHeight};
    SDL_RenderFillRect(renderer, &rect);
}

void DebugWorldGrid::Render(SDL_Renderer *renderer, Math::Camera2D const &camera) const
{
    constexpr float cell_size = Config::Debug::GridCellSize;
    SDL_Color const &color = Config::Debug::GridLineColor;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (float i = 0; i < area_size.x; i += cell_size)
    {
        Math::Vector2 const start{i, 0};
        Math::Vector2 const end{i, area_size.y};
        Math::Vector2 const screen_start = Math::WorldToScreen(start, camera);
        Math::Vector2 const screen_end = Math::WorldToScreen(end, camera);
        SDL_RenderLine(renderer, screen_start.x, screen_start.y, screen_end.x, screen_end.y);
    }

    for (float j = 0; j < area_size.y; j += cell_size)
    {
        Math::Vector2 const start{0, j};
        Math::Vector2 const end{area_size.x, j};
        Math::Vector2 const screen_start = Math::WorldToScreen(start, camera);
        Math::Vector2 const screen_end = Math::WorldToScreen(end, camera);
        SDL_RenderLine(renderer, screen_start.x, screen_start.y, screen_end.x, screen_end.y);
    }
}