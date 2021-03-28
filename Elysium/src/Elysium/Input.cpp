#include "Input.h"

#include <GLFW/glfw3.h>

#include "Elysium/Application.h"

namespace Elysium
{
    namespace Input
    {
        bool isKeyPressed(int keycode)
        {
            auto window = Application::Get().getWindow().getGLFWWindow();
            auto state = glfwGetKey(window, keycode);
            return state == GLFW_PRESS || state == GLFW_REPEAT;
        }

        bool isMouseButtonPressed(int button)
        {
            auto window = Application::Get().getWindow().getGLFWWindow();
            auto state = glfwGetMouseButton(window, button);
            return state == GLFW_PRESS;
        }

        std::pair<float, float> getMousePosition()
        {
            auto window = Application::Get().getWindow().getGLFWWindow();
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            return { (float)x, (float)y };
        }

        float getMouseX()
        {
            auto [x, y] = getMousePosition();
            return x;
        }

        float getMouseY()
        {
            auto [x, y] = getMousePosition();
            return y;
        }
    }
}