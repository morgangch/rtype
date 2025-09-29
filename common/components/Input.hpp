#ifndef COMMON_INPUT_HPP
#define COMMON_INPUT_HPP

#include <ECS/ECS.hpp>
#include <bitset>

namespace rtype::common::components {
    enum class InputAction {
        MoveUp = 0, MoveDown, MoveLeft, MoveRight,
        Fire, Special, Pause, COUNT
    };

    class Input : public ECS::Component<Input> {
    public:
        std::bitset<static_cast<size_t>(InputAction::COUNT)> currentState;
        std::bitset<static_cast<size_t>(InputAction::COUNT)> previousState;

        Input();
    };
}

#endif // COMMON_INPUT_HPP
