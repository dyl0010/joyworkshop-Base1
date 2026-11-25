#pragma once

#include <SDL3/SDL.h>

#include <array>
#include <map>
#include <set>
#include <string>

//
// SDL Input Layer
//
enum class InputState
{
    Pressed,
    Held,
    Released,
    Up,
};

struct InputStatePair
{
    std::string input;
    InputState state;
};

struct MousePosition
{
    float posx;
    float posy;
};

enum Key
{
    W,
    A,
    S,
    D,
    ArrowUp,
    ArrowDown,
    ArrowLeft,
    ArrowRight,
    Space,
    F1,
    F2,
    F3,
    F4,
    KeyMax,
};

enum Mouse
{
    ButtonLeft,
    ButtonRight,
    MouseMax,
};

//
// Gameplay Input Layer
//
enum class InputAction
{
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    ToggleDebugOverlay,
    ToggleCollisionLogs,
    ToggleDebugBounds,
    ToggleWorldGrid,
    ThrowGrenade,
};

class InputManager
{
    MousePosition mouse_position{};

    std::map<SDL_Keycode, Key> keycode_to_key;
    std::map<Uint8, Mouse> mousecode_to_mouse;

    InputStatePair key_states[Key::KeyMax]{};
    InputStatePair mouse_button_states[Mouse::MouseMax]{};

    std::map<InputAction, std::set<Key>> input_action_to_keys;

    std::string InputStateStr(InputState state) const;

    bool IsKeyPressed(Key key) const;
    bool IsKeyReleased(Key key) const;
    bool IsKeyHeld(Key key) const;
    bool IsKeyDown(Key key) const;

    bool IsMouseButtonPressed(Mouse mouse) const;
    bool IsMouseButtonReleased(Mouse mouse) const;
    bool IsMouseButtonHeld(Mouse mouse) const;
    bool IsMouseButtonDown(Mouse mouse) const;

public:
    InputManager();

    void PrintInputState() const;
    void HandleInput(SDL_Event const& event, bool& running);
    void CleanInput();

    bool IsActionPressed(InputAction action) const;
    bool IsActionReleased(InputAction action) const;
    bool IsActionHeld(InputAction action) const;
    bool IsActionDown(InputAction action) const;

    void UnbindKey(InputAction action, Key key);
    void UnbindAllKeys(InputAction action);

    bool IsKeyBound(InputAction action, Key key) const;
    bool IsKeyBoundToAnyAction(Key key) const;
    bool BindKey(InputAction action, Key key);
    bool RebindKey(InputAction action, Key old_key, Key new_key);

    std::set<Key> const& GetBoundKeys(InputAction action) const;
    bool HasAnyBinding(InputAction action) const;
    bool HasAllBindings(InputAction action, std::set<Key> const& keys) const;

    bool IsKeyboardEvent(SDL_Event const &event) const;
    bool IsMouseEvent(SDL_Event const &event) const;
};
