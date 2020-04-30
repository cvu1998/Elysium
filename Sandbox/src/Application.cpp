#include "Layers/SandboxLayer.h"
#include "Layers/TestLayer.h"

class Application : public Elysium::Application
{
private:
    SandboxLayer* m_Sandbox;
    TestLayer* m_Tests;

    bool m_RunSandbox = true;

public:
    Application(bool imgui=false) : Elysium::Application(imgui)
    {
        m_Window->setVSync(false);

        m_Sandbox = new SandboxLayer(m_Window->getWidth(), m_Window->getHeight());
        m_Tests = new TestLayer(&m_RunSandbox);

        m_LayerStack.pushLayer(m_Tests);
        m_LayerStack.pushOverlay(m_Sandbox);
    }

    ~Application()
    {
    }

    void ApplicationLogic() override
    {
        if (m_RunSandbox && !m_LayerStack.contains(m_Sandbox))
            m_LayerStack.pushOverlay(m_Sandbox);
        else if (!m_RunSandbox)
            m_LayerStack.popOverlay(m_Sandbox);
    }
};

int main(void)
{
    Application* application = new Application(true);
    application->RunWithImGui();
    delete application;
    return 0;
}
