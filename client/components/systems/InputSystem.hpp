#pragma once
#include <common/core/System.hpp>
#include <unordered_map>

namespace Client {
    enum class KeyState {
        Released = 0,
        JustPressed,
        Held,
        JustReleased
    };

    class InputSystem : public System {
    public:
        void update(ComponentManager& cm, float deltaTime) override;
        void handleKeyEvent(int key, bool pressed);
        
        bool isKeyPressed(int key) const;
        bool isKeyJustPressed(int key) const;
        
    private:
        std::unordered_map<int, KeyState> keyStates;
        void updateKeyStates();
    };
}
