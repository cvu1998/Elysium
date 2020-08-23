#include "Layers/Connect4Layer.h"
#include "Layers/RLConnect4Layer.h"
#include "Layers/PerformanceLayer.h"
#include "Layers/SandboxLayer.h"
#include "Layers/SoccerLayer.h"
#include "Layers/TestLayer.h"
#include "Layers/RLTTTLayer.h"

class Application : public Elysium::Application
{
private:
    Connect4Layer* m_Connect4 = nullptr;
    RLConnect4Layer* m_RLConnect4 = nullptr;
    PerformanceLayer* m_Test = nullptr;
    SandboxLayer* m_Sandbox = nullptr;
    SoccerLayer* m_Soccer = nullptr;
    TestLayer* m_Tests;
    RLTTTLayer* m_TTT = nullptr;

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
        /*
        if (ImGui::Button("RLConnect4"))
        {
            deleteLayers();

            m_RLConnect4 = new RLConnect4Layer(&m_RunLayer, m_Window->getWidth(), m_Window->getHeight());
            m_LayerStack.pushOverlay(m_RLConnect4);
        }
        */
        if (ImGui::Button("RL Tic-Tac-Toe"))
        {
            deleteLayers();

            m_TTT = new RLTTTLayer(&m_RunLayer, m_Window->getWidth(), m_Window->getHeight());
            m_LayerStack.pushOverlay(m_TTT);
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

        if (m_RLConnect4)
        {
            m_LayerStack.popOverlay(m_RLConnect4);
            delete m_RLConnect4;
            m_RLConnect4 = nullptr;
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

        if (m_TTT)
        {
            m_LayerStack.popOverlay(m_TTT);
            delete m_TTT;
            m_TTT = nullptr;
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
