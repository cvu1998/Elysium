#include "SandboxLayer.h"
#include "test/ClearColor_Test.h"
#include "test/Texture2D_Test.h"
#include "test/ScreenSaver_Test.h"
#include "test/BatchRendering_Test.h"
#include "test/DynamicBatchRendering_Test.h"

class Application : public Elysium::Application
{
private:
    test::Test* m_CurrentTest;
    test::TestMenu* m_TestMenu;

    SandboxLayer* m_Sandbox;

public:
    Application(bool imgui=false) : Elysium::Application(imgui)
    {
        //m_Window->setVSync(false);
        m_TestMenu = new test::TestMenu(m_CurrentTest);
        m_CurrentTest = m_TestMenu;

        m_TestMenu->registerTest<test::ClearColor_Test>("Clear Color");
        m_TestMenu->registerTest<test::Texture2D_Test>("2D Texture");
        m_TestMenu->registerTest<test::ScreenSaver_Test>("Screen Saver");
        m_TestMenu->registerTest<test::BatchRendering_Test>("Batch Rendering");
        m_TestMenu->registerTest<test::DynamicBatchRendering_Test>("Dynamic Batch Rendering");

        m_Sandbox = new SandboxLayer(m_Window->getWidth(), m_Window->getHeight());

        m_LayerStack.pushOverlay(m_Sandbox);
    }

    ~Application()
    {
        delete m_CurrentTest;
        if (m_CurrentTest != m_TestMenu)
            delete m_TestMenu;
    }

    void ApplicationLogic() override
    {
        if (m_CurrentTest)
        {
            if (m_CurrentTest == m_TestMenu && m_LayerStack.begin() == m_LayerStack.end())
                m_LayerStack.pushOverlay(m_Sandbox);

            m_CurrentTest->onUpdate(0.0f);
            m_CurrentTest->onRender();
            ImGui::Begin("Test");
            if (m_CurrentTest != m_TestMenu)
            {
                if (m_LayerStack.begin() != m_LayerStack.end())
                    m_LayerStack.popOverlay(m_Sandbox);

                if (ImGui::Button("<--"))
                {
                    delete m_CurrentTest;
                    m_CurrentTest = m_TestMenu;
                }
            }
            m_CurrentTest->onImGuiRender();
            ImGui::End();
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
