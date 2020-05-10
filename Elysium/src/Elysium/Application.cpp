#include "Application.h"

#include <algorithm>
#include <iostream>

#include "Elysium/Renderer/Renderer.h"
#include "Elysium/Timestep.h"

namespace Elysium
{
    Application* Application::s_Instance = nullptr;

    Application::Application(bool imgui) : m_ImGui(imgui), m_ClearColor{ 0.0f, 0.0f, 0.0f, 0.0f }
    {
        LOG_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->setEventCallback(BIND_EVENT_FUNCTION(Application::onEvent));

        if (m_Window->getStatus() == 0)
            std::cout << "Glad Init Error!" << "\n";
        
        #ifdef _DEBUG
        std::cout << glGetString(GL_VERSION) << "\n";
        #endif

        if (m_ImGui)
        {
            const char* glsl_version = "#version 130";
            ImGui::CreateContext();
            ImGui_ImplGlfw_InitForOpenGL(m_Window->getGLFWWindow(), true);
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
    }

    void Application::onEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNCTION(Application::onWindowCloseEvent));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNCTION(Application::onWindowResizeEvent));

        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
        {
            (*--it)->onEvent(event);
            if (event.Handled)
                break;
        }
    }

    void Application::pushLayer(Layer* layer)
    {
        m_LayerStack.pushLayer(layer);
    }

    void Application::pushOverlay(Layer* overlay)
    {
        m_LayerStack.pushOverlay(overlay);
    }

    bool Application::onWindowCloseEvent(WindowCloseEvent& event)
    {
        m_Running = false;
        return true;
    }

    bool Application::onWindowResizeEvent(WindowResizeEvent& event)
    {
        if (event.getWidth() == 0 || event.getHeight() == 0)
        {
            m_Minimized = true;
            return false;
        }
        m_Minimized = false;

        Renderer::setViewport(0, 0, event.getWidth(), event.getHeight());

        return false;
    }

    void Application::Run()
    {
        while (m_Running)
        {
            Renderer::Clear({ m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3] });

            float time = (float)glfwGetTime();
            Timestep timestep = std::min(0.0333f, time - m_LastFrameTime);
            m_LastFrameTime = time;

            if (!m_Minimized) 
            {
                this->ApplicationLogic();

                for (Layer* layer : m_LayerStack)
                    layer->onUpdate(timestep);
            }

            m_Window->onUpdate();
        }
    }

    void Application::RunWithImGui()
    {
        while (m_Running)
        {
            Renderer::Clear({ m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3] });

            float time = (float)glfwGetTime();
            Timestep timestep = std::min(0.0333f, time - m_LastFrameTime);
            m_LastFrameTime = time;

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            if (!m_Minimized)
            {
                this->ApplicationLogic();

                for (Layer* layer : m_LayerStack)
                    layer->onUpdate(timestep);
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            m_Window->onUpdate();
        }
    }

    void Application::setClearColor(float r, float g, float b, float a)
    {
        m_ClearColor[0] = r;
        m_ClearColor[1] = g;
        m_ClearColor[2] = b;
        m_ClearColor[3] = a;
    }
}
