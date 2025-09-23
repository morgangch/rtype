#pragma once
#include <ECS/ECS.hpp>
#include <bitset>

namespace Common {
    enum class InputAction {
        MoveUp = 0, MoveDown, MoveLeft, MoveRight, Fire, Special, Pause, COUNT
    };

    class Input : public ECS::Component<Input> {
    public:
        std::bitset<static_cast<size_t>(InputAction::COUNT)> currentState;
        std::bitset<static_cast<size_t>(InputAction::COUNT)> previousState;

        Input();
    };
}
