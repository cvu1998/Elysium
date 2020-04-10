#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

namespace Elysium 
{
    class Application
    {
    protected:
        GLFWwindow* m_Window;
        bool m_ImGui;

        Application(bool imgui=false);
        ~Application();

        virtual void ApplicationLogic() = 0;

    public:
        virtual void Run() final;
        virtual void RunWithImGui() final;
    };
}

