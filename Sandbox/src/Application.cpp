#include "Layers/SandboxLayer.h"
#include "Layers/TestLayer.h"

class Application : public Elysium::Application
{
private:
    SandboxLayer* m_Sandbox;
    TestLayer* m_Tests;

    bool m_VSync = true;

    bool m_RunSandbox = true;

public:
    Application(bool imgui=false) : Elysium::Application(imgui)
    {
        m_Window->setVSync(m_VSync);

        m_Sandbox = new SandboxLayer(&m_RunSandbox, m_Window->getWidth(), m_Window->getHeight());
        m_Tests = new TestLayer(&m_RunSandbox);

        m_LayerStack.pushLayer(m_Tests);
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
