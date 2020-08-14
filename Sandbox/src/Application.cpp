#include "Layers/Connect4Layer.h"
#include "Layers/PerformanceLayer.h"
#include "Layers/SandboxLayer.h"
#include "Layers/SoccerLayer.h"
#include "Layers/TestLayer.h"

class Application : public Elysium::Application
{
private:
    Connect4Layer* m_Connect4 = nullptr;
    PerformanceLayer* m_Test = nullptr;
    SandboxLayer* m_Sandbox = nullptr;
    SoccerLayer* m_Soccer = nullptr;
    TestLayer* m_Tests;

    bool m_VSync = true;

    bool m_RunLayer = true;

public:
    Application(bool imgui=false) : Elysium::Application(imgui)
    {
        m_Window->setVSync(m_VSync);

        m_Tests = new TestLayer(&m_RunLayer);

        m_LayerStack.pushLayer(m_Tests);
    }

    ~Application()
    {
    }

    void ApplicationLogic() override
    {
        ImGui::Begin("Main Application");
        ImGui::Checkbox("VSync", &m_VSync);
        ImGui::ColorEdit4("Clear Color", m_ClearColor);
        if (ImGui::Button("Sandbox"))
        {
            deleteLayers();
            
            m_Sandbox = new SandboxLayer(&m_RunLayer, m_Window->getWidth(), m_Window->getHeight());
            m_LayerStack.pushOverlay(m_Sandbox);
        }
        if (ImGui::Button("Stress Test"))
        {
            deleteLayers();

            m_Test = new PerformanceLayer(&m_RunLayer, m_Window->getWidth(), m_Window->getHeight());
            m_LayerStack.pushOverlay(m_Test);
        }
        if (ImGui::Button("Connect 4"))
        {
            deleteLayers();

            m_Connect4 = new Connect4Layer(&m_RunLayer, m_Window->getWidth(), m_Window->getHeight());
            m_LayerStack.pushOverlay(m_Connect4);
        }
        if (ImGui::Button("Soccer"))
        {
            deleteLayers();

            m_Soccer = new SoccerLayer(&m_RunLayer, m_Window->getWidth(), m_Window->getHeight());
            m_LayerStack.pushOverlay(m_Soccer);
        }
        ImGui::End();

        m_Window->setVSync(m_VSync);
    }

    void deleteLayers()
    {
        if (m_Connect4)
        {
            m_LayerStack.popOverlay(m_Connect4);
            delete m_Connect4;
            m_Connect4 = nullptr;
        }

        if (m_Sandbox)
        {
            m_LayerStack.popOverlay(m_Sandbox);
            delete m_Sandbox;
            m_Sandbox = nullptr;
        }

        if (m_Soccer)
        {
            m_LayerStack.popOverlay(m_Soccer);
            delete m_Soccer;
            m_Soccer = nullptr;
        }

        if (m_Test)
        {
            m_LayerStack.popOverlay(m_Test);
            delete m_Test;
            m_Test = nullptr;
        }
    }
};

int main(void)
{
    Application* application = new Application(true);
    application->RunWithImGui();
    delete application;
    return 0;
}
