#pragma once

#include <Utility>

namespace Elysium
{
    class Input
    {
    private:
        static Input* s_Instance;

        bool isKeyPressedImpl(int keycode);
        bool isMouseButtonPressedImpl(int button);
        std::pair<float, float> getMousePositionImpl();
        float getMouseXImpl();
        float getMouseYImpl();

    public:
        inline static bool isKeyPressed(int keycode) { return s_Instance->isKeyPressedImpl(keycode); }
        inline static bool isMouseButtonPressed(int button) { return s_Instance->isMouseButtonPressedImpl(button); }
        inline static std::pair<float, float> getMousePosition() { return s_Instance->getMousePositionImpl(); }
        inline static float getMouseX() { return s_Instance->getMouseXImpl(); }
        inline static float getMouseY() { return s_Instance->getMouseYImpl(); }
    };
}