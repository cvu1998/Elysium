#include "Layers/GameLayer.h"
#include "Layers/SandboxLayer.h"
#include "Layers/TestLayer.h"

class Application : public Elysium::Application
{
private:
    GameLayer* m_Game;
    SandboxLayer* m_Sandbox;
    TestLayer* m_Tests;

    bool m_VSync = true;

    bool m_RunLayer = true;

public:
    Application(bool imgui=false) : Elysium::Application(imgui)
    {
        m_Window->setVSync(m_VSync);

        //m_Game = new GameLayer(&m_RunLayer, m_Window->getWidth(), m_Window->getHeight());
        m_Sandbox = new SandboxLayer(&m_RunLayer, m_Window->getWidth(), m_Window->getHeight());
        m_Tests = new TestLayer(&m_RunLayer);

        m_LayerStack.pushLayer(m_Tests);
        //m_LayerStack.pushOverlay(m_Game);
        m_LayerStack.pushOverlay(m_Sandbox);
    }

    ~Application()
    {
    }

    void ApplicationLogic() override
    {
        ImGui::Begin("Main Application");
        ImGui::Checkbox("VSync", &m_VSync);
        ImGui::ColorEdit4("Clear Color", m_ClearColor);
        if (ImGui::Button("Play Again!"))
        {
            m_LayerStack.popOverlay(m_Sandbox);
            delete m_Sandbox;
            
            m_Sandbox = new SandboxLayer(&m_RunLayer, m_Window->getWidth(), m_Window->getHeight());
            m_LayerStack.pushOverlay(m_Sandbox);

            //m_LayerStack.popOverlay(m_Game);
            //delete m_Game;
            //
            //m_Game = new GameLayer(&m_RunLayer, m_Window->getWidth(), m_Window->getHeight());
            //m_LayerStack.pushOverlay(m_Game);
        }
        ImGui::End();

        m_Window->setVSync(m_VSync);
    }
};

int main(void)
{
    Application* application = new Application(true);
    application->RunWithImGui();
    delete application;
    return 0;
}
