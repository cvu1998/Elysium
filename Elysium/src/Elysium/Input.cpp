#include "Input.h"

#include <GLFW/glfw3.h>

#include "Elysium/Application.h"

namespace Elysium
{
    Input* Input::s_Instance = new Input();

    bool Input::isKeyPressedImpl(int keycode)
    {
        auto window = Application::Get().getWindow().getGLFWWindow();
        auto state = glfwGetKey(window, keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool Input::isMouseButtonPressedImpl(int button)
    {
        auto window = Application::Get().getWindow().getGLFWWindow();
        auto state = glfwGetMouseButton(window, button);
        return state == GLFW_PRESS;
    }

    std::pair<float, float> Input::getMousePositionImpl()
    {
        auto window = Application::Get().getWindow().getGLFWWindow();
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        return { (float)x, (float)y };
    }

    float Input::getMouseXImpl()
    {
        auto [x, y] = getMousePositionImpl();
        return x;
    }

    float Input::getMouseYImpl()
    {
        auto [x, y] = getMousePositionImpl();
        return y;
    }
}