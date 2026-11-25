#pragma once

#include <SDL3/SDL.h>

#include <array>
#include <cstddef>

#include "Math.h"

namespace Config
{

constexpr int DefaultWindowWidth = 1280;
constexpr int DefaultWindowHeight = 720;
inline constexpr char DefaultWindowTitle[] = "DemoGame";

namespace Assets
{

inline constexpr char PlayerSpriteSheetPath[] = "./assets/directions.bmp";

} // namespace Assets

namespace World
{
constexpr int Width = 2400;
constexpr int Height = 1600;
}  // namespace World

namespace Player
{
    constexpr float SpriteWidth = 64.f;
    constexpr float SpriteHeight = 64.f;
    constexpr int MoveSpeed = 240;
    constexpr double AnimationRate = 0.1;
    constexpr Math::Vector2 StartPoint{640.f, 360.f};

    // sprite sheet layout
    // 0    1    2   3     // Up
    // 4    5    6   7     // Down
    // 8    9    10  11    // Left
    // 12   13   14  15    // Right
    constexpr std::array<std::size_t, 4> UpAnimationIndices{0, 1, 2, 3};
    constexpr std::array<std::size_t, 4> DownAnimationIndices{4, 5, 6, 7};
    constexpr std::array<std::size_t, 4> LeftAnimationIndices{8, 9, 10, 11};
    constexpr std::array<std::size_t, 4> RightAnimationIndices{12, 13, 14, 15};

    constexpr std::size_t SpriteSheetColumns = 4;
    constexpr std::size_t AnimationFrameCount = 4;

    inline constexpr char DebugName[] = "Player";
}  // namespace Player

namespace Timing
{
    constexpr double FixedUpdatesPerSecond = 60.0;
    constexpr double FixedDelta = 1.0 / FixedUpdatesPerSecond;
    constexpr double MaxFrameDelta = 0.25;
    constexpr int MaxFixedUpdatesPerFrame = 5;
}  // namespace Timing

namespace Debug
{
    constexpr float GridCellSize = 64;
    constexpr SDL_Color GridLineColor{80, 80, 80, 255};
    constexpr Math::Vector2 MarkerStartPoint{100, 100};
    constexpr SDL_Color MarkerColor{255, 0, 0, 255};
    constexpr float MarkerWidth = 10;
    constexpr float MarkerHeight = 10;
    
    constexpr bool EnableCollisionLogs = true;
    constexpr bool EnableCollectibleLogs = true;
    constexpr bool EnableTriggerLogs = true;

    constexpr bool ShowDebugBounds = true;
    constexpr SDL_Color CollisionBoundsColor{0, 255, 0, 255};
    constexpr SDL_Color TriggerBoundsColor{0, 255, 255, 255};

    constexpr bool ShowWorldGrid = true;
    constexpr bool ShowWorldMarker = true;

    constexpr bool EnableFrameTimingLogs = false;
    constexpr bool EnableDroppedFrameTimingLogs = false;
} // namespace Debug

namespace Blocks
{
    constexpr char GreenBlockDebugName[] = "GreenBlock";
    constexpr Math::Transform PurpleBlockTransform{{633, 353}, {5, 5}};
    constexpr SDL_Color PurpleBlockColor{255, 0, 255, 255};

    constexpr char BlueBlockDebugName[] = "BlueBlock";
    constexpr Math::Transform BlueBlockTransform{{200, 200}, {50*3, 50*3}};
    constexpr SDL_Color BlueBlockColor{0, 0, 255, 255};
    
    constexpr char YellowBlockDebugName[] = "YellowBlock";
    constexpr Math::Transform YellowBlockTransform{{50, 300}, {25, 50*5}};
    constexpr SDL_Color YellowBlockColor{255, 255, 0, 255};
    
    constexpr char RedBlockDebugName[] = "RedBlock";
    constexpr Math::Transform RedBlockTransform{{300, 500}, {50*5, 25}};
    constexpr SDL_Color RedBlockColor{255, 0, 0, 255};
} // namespace Blocks

namespace Collectibles
{
    constexpr char CoinDebugName[] = "Coin";
    constexpr Math::Transform CoinTransform{{900, 360}, {24, 24}};
    constexpr SDL_Color CoinColor{255, 215, 0, 255};
    constexpr SDL_Color CoinBoundsColor{255, 0, 0, 255};

    constexpr char GemDebugName[] = "Gem";
    constexpr Math::Transform GemTransform{{1000, 360}, {36, 36}};
    constexpr SDL_Color GemColor{255, 0, 0, 255};
    constexpr SDL_Color GemBoundsColor{255, 215, 0, 255};
    
    constexpr int RequiredCount = 2;
} // namespace Collectibles

namespace Doors
{
    constexpr char ExitDoorDebugName[] = "ExitDoor";
    constexpr Math::Transform ExitDoorTransform{{800, 450}, {15, 120}};
    constexpr SDL_Color ExitDoorColor{150, 90, 60, 255};
}  // namespace Doors

namespace PresetColor
{
    constexpr SDL_Color White = {255, 255, 255, 255};
    constexpr SDL_Color Grey = {100, 100, 100, 255};
    constexpr SDL_Color Red = {255, 0, 0, 255};
    constexpr SDL_Color Green = {0, 255, 0, 255};
    constexpr SDL_Color Blue = {0, 0, 255, 255};
    constexpr SDL_Color Yellow = {255, 255, 0, 255};
    constexpr SDL_Color Purple = {63, 72, 204, 255};
    constexpr SDL_Color Background = {34, 34, 34, 255};
} // namespace PresetColors

} // namespace Config
