#include "Grenade.h"

#include <cmath>

#include <glm/glm.hpp>

#include "Config.h"

Grenade::Grenade(Math::Transform const &transform,
                 Math::Vector2 const &target_position,
                 SDL_Color const &color,
                 SDL_Color const &debug_bounds_color,
                 char const *debug_name)
: Entity{transform, debug_name},
  target_position{target_position},
  color{color},
  debug_bounds_color{debug_bounds_color}
{

}

void Grenade::Update(double frame_delta)
{
    if (finished)
    {
        return;
    }

    if (!landed)
    {
        Math::Vector2 to_target {
            target_position.x - transform.position.x,
            target_position.y - transform.position.y
        };

        float const distance_to_target = Math::Length(to_target);
        float const step = travel_speed * static_cast<float>(frame_delta);

        if (distance_to_target <= step)
        {
            SetPosition(target_position);
            landed = true;
            return;
        }

        Math::Normalize(to_target);
        SetPosition({
            transform.position.x + to_target.x * step,
            transform.position.y + to_target.y * step,
        });
    }
    else
    {
        elapsed += frame_delta;
        if (!exploded && elapsed >= fuse_seconds)
        {
            exploded = true;
        }

        if (!finished && exploded)
        {
            explosion_elapsed += frame_delta;

            if (explosion_elapsed >= explosion_visible_seconds)
            {
                finished = true;
                SetActive(false);
            }
        }
    }
}

void Grenade::Render(SDL_Renderer* renderer, Math::Camera2D const& camera) const
{
    if (finished)
    {
        return;
    }

    if (exploded)
    {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        Math::Vector2 exploded_position = Math::WorldToScreen(GetCenter(), camera);
        SDL_FRect exploded_rect{exploded_position.x - radius, exploded_position.y - radius, 2*radius, 2*radius};
        SDL_RenderRect(renderer, &exploded_rect);
    }
    else
    {
        SDL_Color render_color = color;
        if (landed)
        {
            int const blink_phase = static_cast<int>(elapsed / blink_interval);
            if (blink_phase % 2 == 1)
            {
                render_color = warning_color;
            }
        }

        SDL_SetRenderDrawColor(renderer, render_color.r, render_color.g, render_color.b, render_color.a);
        Math::Vector2 screen_position = Math::WorldToScreen(transform.position, camera);
        SDL_FRect rect{screen_position.x, screen_position.y, transform.size.x, transform.size.y};
        SDL_RenderFillRect(renderer, &rect);
        
        SDL_Color const line_color = Config::PresetColor::Blue;
        SDL_SetRenderDrawColor(renderer, line_color.r, line_color.g, line_color.b, line_color.a);
        Math::Vector2 target_screen_position = Math::WorldToScreen(target_position, camera);
        SDL_RenderLine(renderer,
            screen_position.x + transform.size.x * 0.5f,
            screen_position.y + transform.size.y * 0.5f,
            target_screen_position.x + transform.size.x * 0.5f,
            target_screen_position.y + transform.size.y * 0.5f);
    }
}

bool Grenade::IsFinished() const
{
    return finished;
}