#include "Application.h"

#include <iostream>

#include "Renderer.h"

namespace Elysium
{
    Application::Application(bool imgui) : m_ImGui(imgui)
    {
        if (!glfwInit())
            std::cout << "Error: glfwInit" << std::endl;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

        m_Window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL);
        if (!m_Window)
        {
            glfwTerminate();
            std::cout << "Error: glfwCreateWindow" << std::endl;
        }

        glfwMakeContextCurrent(m_Window);
        glfwSwapInterval(1);

        if (glewInit() != GLEW_OK)
            std::cout << "Glew Init Error!" << std::endl;
        std::cout << glGetString(GL_VERSION) << std::endl;

        if (m_ImGui)
        {
            const char* glsl_version = "#version 130";
            ImGui::CreateContext();
            ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
            ImGui_ImplOpenGL3_Init(glsl_version);
            ImGui::StyleColorsDark();
        }
    }

    Application::~Application()
    {
        if (m_ImGui)
        {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void Application::Run()
    {
        while (!glfwWindowShouldClose(m_Window))
        {
            Renderer::clear();

            if (m_ImGui)
            {
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();
            }

            this->ApplicationLogic();

            if (m_ImGui)
            {
                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            }

            glfwSwapBuffers(m_Window);
            glfwPollEvents();
        }
    }

    void Application::ApplicationLogic()
    {

    }
}
