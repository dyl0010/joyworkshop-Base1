#include "Bomb.h"

Bomb::Bomb(Math::Transform const &transform, char const *debug_name)
    : Entity{transform, debug_name}
{
    SetCollisionEnabled(true);
}


void Bomb::Update(double frame_delta)
{
    if (finished)
    {
        return;
    }

    if (!exploded)
    {
        elapsed += frame_delta;
        if (elapsed >= fuse_seconds)
        {
            exploded = true;
            SetCollisionEnabled(false);
        }
    }
    else
    {
        explosion_elapsed += frame_delta;
        if (explosion_elapsed >= explosion_visible_seconds)
        {
            finished = true;
            SetActive(false);
        }
    }
}

void Bomb::Render(SDL_Renderer* renderer, Math::Camera2D const& camera) const
{
    if (finished)
    {
        return;
    }

    if (exploded)
    {
        SDL_SetRenderDrawColor(renderer, explosion_color.r, explosion_color.g, explosion_color.b, explosion_color.a);
        std::vector<Math::Rect> const explosion_bounds = GetExplosionBounds();
        for (Math::Rect const &rect: explosion_bounds)
        {
            Math::Vector2 const screen_position = Math::WorldToScreen(rect.position, camera);
            SDL_FRect const screen_rect{screen_position.x, screen_position.y, rect.size.x, rect.size.y};
            SDL_RenderRect(renderer, &screen_rect);
        }
    }
    else
    {
        SDL_Color render_color = fuse_color;
        int const blink_phase = static_cast<int>(elapsed / blink_interval);
        if (blink_phase % 2 == 1)
        {
            render_color = warning_color;
        }

        SDL_SetRenderDrawColor(renderer, render_color.r, render_color.g, render_color.b, render_color.a);
        Math::Vector2 screen_position = Math::WorldToScreen(transform.position, camera);
        SDL_FRect rect{screen_position.x, screen_position.y, transform.size.x, transform.size.y};
        SDL_RenderFillRect(renderer, &rect);
    }
}

bool Bomb::IsFinished() const
{
    return finished;
}

bool Bomb::HasExploded() const
{
    return exploded;
}

std::vector<Math::Rect> Bomb::GetExplosionBounds() const
{
    if (!exploded || finished)
    {
        return {};
    }

    std::vector<Math::Rect> explosion_bounds{};

    auto collect_cell = [&explosion_bounds, this](int offset_x, int offset_y)
    {
        Math::Rect const rect {
            {
                transform.position.x + offset_x * tile_size,
                transform.position.y + offset_y * tile_size
            },
            transform.size
        };

        explosion_bounds.push_back(rect);
    };

    collect_cell(0, 0);
    for (int i = 1; i <= explosion_range; ++i)
    {
        collect_cell(0, -i);
        collect_cell(0, i);
        collect_cell(-i, 0);
        collect_cell(i, 0);
    }

    return explosion_bounds;
}

int Bomb::GetExplosionRange() const
{
    return explosion_range;
}