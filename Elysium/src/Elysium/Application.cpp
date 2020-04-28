#include "Application.h"

#include <iostream>

#include "Renderer.h"

namespace Elysium
{
#define BIND_EVENT_FUNCTION(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application::Application(bool imgui) : m_ImGui(imgui)
    {
        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->setEventCallback(BIND_EVENT_FUNCTION(onEvent));

        if (glewInit() != GLEW_OK)
            std::cout << "Glew Init Error!" << "\n";
        std::cout << glGetString(GL_VERSION) << "\n";

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
        #ifdef _DEBUG
        std::cout << event << "\n";
        #endif

        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNCTION(onWindowCloseEvent));

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

    void Application::Run()
    {
        while (m_Running)
        {
            Renderer::clear();

            this->ApplicationLogic();

            for (Layer* layer : m_LayerStack)
                layer->onUpdate();

            m_Window->onUpdate();
        }
    }

    void Application::RunWithImGui()
    {
        while (m_Running)
        {
            Renderer::clear();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            this->ApplicationLogic();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            m_Window->onUpdate();
        }
    }
}
