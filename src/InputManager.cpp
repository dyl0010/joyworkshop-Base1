#include "InputManager.h"

#include <iostream>

InputManager::InputManager()
{
    keycode_to_key.insert({
        {SDLK_W, Key::W},
        {SDLK_A, Key::A},
        {SDLK_S, Key::S},
        {SDLK_D, Key::D},
        {SDLK_UP, Key::ArrowUp},
        {SDLK_DOWN, Key::ArrowDown},
        {SDLK_LEFT, Key::ArrowLeft},
        {SDLK_RIGHT, Key::ArrowRight},
        {SDLK_SPACE, Key::Space},
        {SDLK_F1, Key::F1},
        {SDLK_F2, Key::F2},
        {SDLK_F3, Key::F3},
        {SDLK_F4, Key::F4},
    });

    mousecode_to_mouse.insert({
        {SDL_BUTTON_LEFT, Mouse::ButtonLeft},
        {SDL_BUTTON_RIGHT, Mouse::ButtonRight},
    });

    key_states[Key::W] = {"W", InputState::Up};
    key_states[Key::A] = {"A", InputState::Up};
    key_states[Key::S] = {"S", InputState::Up};
    key_states[Key::D] = {"D", InputState::Up};
    key_states[Key::ArrowUp] = {"ArrowUp", InputState::Up};
    key_states[Key::ArrowDown] = {"ArrowDown", InputState::Up};
    key_states[Key::ArrowLeft] = {"ArrowLeft", InputState::Up};
    key_states[Key::ArrowRight] = {"ArrowRight", InputState::Up};
    key_states[Key::Space] = {"Space", InputState::Up};
    key_states[Key::F1] = {"F1", InputState::Up};
    key_states[Key::F2] = {"F2", InputState::Up};
    key_states[Key::F3] = {"F3", InputState::Up};
    key_states[Key::F4] = {"F4", InputState::Up};

    mouse_button_states[Mouse::ButtonLeft] = {"Left Button", InputState::Up};
    mouse_button_states[Mouse::ButtonRight] = {"Right Button", InputState::Up};

    input_action_to_keys.insert({
        {InputAction::MoveUp, {Key::W, Key::ArrowUp}},
        {InputAction::MoveDown, {Key::S, Key::ArrowDown}},
        {InputAction::MoveLeft, {Key::A, Key::ArrowLeft}},
        {InputAction::MoveRight, {Key::D, Key::ArrowRight}},
        {InputAction::ThrowGrenade, {Key::Space}},
        {InputAction::ToggleDebugOverlay, {Key::F1}},
        {InputAction::ToggleCollisionLogs, {Key::F2}},
        {InputAction::ToggleDebugBounds, {Key::F3}},
        {InputAction::ToggleWorldGrid, {Key::F4}},
    });
}

std::string InputManager::InputStateStr(InputState state) const
{
    if (state == InputState::Pressed)
    {
        return "Pressed";
    }
    if (state == InputState::Held)
    {
        return "Held";
    }
    if (state == InputState::Released)
    {
        return "Released";
    }

    return "Up";
}

void InputManager::PrintInputState() const
{
    std::cout << "-------------\n";
    for (int i = 0; i < Key::KeyMax; ++i)
    {
        InputStatePair const current = key_states[i];
        std::cout << current.input << ": " << InputStateStr(current.state) << '\n';
    }

    for (int i = 0; i < Mouse::MouseMax; ++i)
    {
        InputStatePair const current = mouse_button_states[i];
        std::cout << current.input << ": " << InputStateStr(current.state) << '\n';
    }

    std::cout << "Mouse X: " << mouse_position.posx << '\n';
    std::cout << "Mouse Y: " << mouse_position.posy << '\n';
    std::cout << std::flush;
}

void InputManager::HandleInput(SDL_Event const& event, bool& running)
{
    if (event.type == SDL_EVENT_MOUSE_MOTION)
    {
        mouse_position.posx = event.motion.x;
        mouse_position.posy = event.motion.y;
    }
    else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        if (auto const iter = mousecode_to_mouse.find(event.button.button); iter != mousecode_to_mouse.end())
        {
            Mouse const mouse = iter->second;
            mouse_button_states[mouse].state = InputState::Pressed;
        }
    }
    else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
        if (auto const iter = mousecode_to_mouse.find(event.button.button); iter != mousecode_to_mouse.end())
        {
            Mouse const mouse = iter->second;
            mouse_button_states[mouse].state = InputState::Released;
        }
    }
    else if (event.type == SDL_EVENT_QUIT)
    {
        running = false;
    }
    else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
    {
        running = false;
    }
    else if (event.type == SDL_EVENT_KEY_DOWN)
    {
        if (auto const iter = keycode_to_key.find(event.key.key); iter != keycode_to_key.end())
        {
            Key const key = iter->second;
            if (!event.key.repeat)
            {
                key_states[key].state = InputState::Pressed;
            }
        }
    }
    else if (event.type == SDL_EVENT_KEY_UP)
    {
        if (auto const iter = keycode_to_key.find(event.key.key); iter != keycode_to_key.end())
        {
            Key const key = iter->second;
            key_states[key].state = InputState::Released;
        }
    }
}

void InputManager::CleanInput()
{
    for (InputStatePair& key_input : key_states)
    {
        if (key_input.state == InputState::Pressed)
        {
            key_input.state = InputState::Held;
        }
        if (key_input.state == InputState::Released)
        {
            key_input.state = InputState::Up;
        }
    }

    for (InputStatePair& mouse_input : mouse_button_states)
    {
        if (mouse_input.state == InputState::Pressed)
        {
            mouse_input.state = InputState::Held;
        }
        if (mouse_input.state == InputState::Released)
        {
            mouse_input.state = InputState::Up;
        }
    }
}

bool InputManager::IsKeyPressed(Key key) const
{
    return key_states[key].state == InputState::Pressed;
}

bool InputManager::IsKeyReleased(Key key) const
{
    return key_states[key].state == InputState::Released;
}

bool InputManager::IsKeyHeld(Key key) const
{
    return key_states[key].state == InputState::Held;
}

bool InputManager::IsKeyDown(Key key) const
{
    return key_states[key].state == InputState::Pressed || key_states[key].state == InputState::Held;
}

bool InputManager::IsMouseButtonPressed(Mouse mouse) const
{
    return mouse_button_states[mouse].state == InputState::Pressed;
}

bool InputManager::IsMouseButtonReleased(Mouse mouse) const
{
    return mouse_button_states[mouse].state == InputState::Released;
}

bool InputManager::IsMouseButtonHeld(Mouse mouse) const
{
    return mouse_button_states[mouse].state == InputState::Held;
}

bool InputManager::IsMouseButtonDown(Mouse mouse) const
{
    return mouse_button_states[mouse].state == InputState::Pressed ||
        mouse_button_states[mouse].state == InputState::Held;
}

bool InputManager::IsActionPressed(InputAction action) const
{
    for (Key const key : input_action_to_keys.at(action))
    {
        if (IsKeyPressed(key))
        {
            return true;
        }
    }

    return false;
}

bool InputManager::IsActionReleased(InputAction action) const
{
    for (Key const key : input_action_to_keys.at(action))
    {
        if (IsKeyReleased(key))
        {
            return true;
        }
    }

    return false;
}

bool InputManager::IsActionHeld(InputAction action) const
{
    for (Key const key : input_action_to_keys.at(action))
    {
        if (IsKeyHeld(key))
        {
            return true;
        }
    }

    return false;
}

bool InputManager::IsActionDown(InputAction action) const
{
    for (Key const key : input_action_to_keys.at(action))
    {
        if (IsKeyDown(key))
        {
            return true;
        }
    }

    return false;
}

void InputManager::UnbindKey(InputAction action, Key key)
{
    input_action_to_keys.at(action).erase(key);
}

void InputManager::UnbindAllKeys(InputAction action)
{
    input_action_to_keys.at(action).clear();
}

bool InputManager::IsKeyBound(InputAction action, Key key) const
{
    return input_action_to_keys.at(action).contains(key);
}

bool InputManager::IsKeyBoundToAnyAction(Key key) const
{
    for (auto const& [action, keys] : input_action_to_keys)
    {
        (void)keys;
        if (IsKeyBound(action, key))
        {
            return true;
        }
    }

    return false;
}

bool InputManager::BindKey(InputAction action, Key key)
{
    if (!IsKeyBoundToAnyAction(key))
    {
        input_action_to_keys.at(action).insert(key);
        return true;
    }

    return false;
}

bool InputManager::RebindKey(InputAction action, Key old_key, Key new_key)
{
    if (input_action_to_keys.at(action).contains(old_key))
    {
        if (old_key == new_key)
        {
            return true;
        }

        if (BindKey(action, new_key))
        {
            UnbindKey(action, old_key);
            return true;
        }
    }

    return false;
}

std::set<Key> const& InputManager::GetBoundKeys(InputAction action) const
{
    return input_action_to_keys.at(action);
}

bool InputManager::HasAnyBinding(InputAction action) const
{
    return !GetBoundKeys(action).empty();
}

bool InputManager::HasAllBindings(InputAction action, std::set<Key> const& keys) const
{
    std::set<Key> const& bound_keys = GetBoundKeys(action);
    for (Key const key : keys)
    {
        if (!bound_keys.contains(key))
        {
            return false;
        }
    }

    return true;
}

bool InputManager::IsKeyboardEvent(SDL_Event const &event) const
{
    return event.type == SDL_EVENT_KEY_UP || event.type == SDL_EVENT_KEY_DOWN;
}

bool InputManager::IsMouseEvent(SDL_Event const &event) const
{
    return event.type == SDL_EVENT_MOUSE_MOTION ||
           event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
           event.type == SDL_EVENT_MOUSE_BUTTON_UP ||
           event.type == SDL_EVENT_MOUSE_WHEEL;
}