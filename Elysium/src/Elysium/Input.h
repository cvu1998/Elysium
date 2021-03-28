#pragma once

#include <Utility>

namespace Elysium
{
    namespace Input
    {
        bool isKeyPressed(int keycode);
        bool isMouseButtonPressed(int button);
        std::pair<float, float> getMousePosition();
        float getMouseX();
        float getMouseY();
    }
}