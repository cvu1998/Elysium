#include "Layers/PerformanceLayer.h"
#include "Layers/SandboxLayer.h"
#include "Layers/TestLayer.h"

class Application : public Elysium::Application
{
private:
    PerformanceLayer* m_Test = nullptr;
    SandboxLayer* m_Sandbox = nullptr;
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
            if (m_Test)
            {
                m_LayerStack.popOverlay(m_Test);
                delete m_Test;
                m_Test = nullptr;
            }

            if (m_Sandbox)
            {
                m_LayerStack.popOverlay(m_Sandbox);
                delete m_Sandbox;
                m_Sandbox = nullptr;
            }
            
            m_Sandbox = new SandboxLayer(&m_RunLayer, m_Window->getWidth(), m_Window->getHeight());
            m_LayerStack.pushOverlay(m_Sandbox);
        }
        if (ImGui::Button("Stress Test"))
        {
            if (m_Sandbox)
            {
                m_LayerStack.popOverlay(m_Sandbox);
                delete m_Sandbox;
                m_Sandbox = nullptr;
            }

            if (m_Test)
            {
                m_LayerStack.popOverlay(m_Test);
                delete m_Test;
                m_Test = nullptr;
            }

            m_Test = new PerformanceLayer(&m_RunLayer, m_Window->getWidth(), m_Window->getHeight());
            m_LayerStack.pushOverlay(m_Test);
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
