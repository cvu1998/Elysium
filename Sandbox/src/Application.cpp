#include "test/ClearColor_Test.h"
#include "test/Texture2D_Test.h"
#include "test/ScreenSaver_Test.h"
#include "test/BatchRendering_Test.h"
#include "test/DynamicBatchRendering_Test.h"
#include "test/Sandbox.h"

class Application : public Elysium::Application
{
private:
    test::Test* m_CurrentTest;
    test::TestMenu* m_TestMenu;

    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_ViewMatrix;

public:
    Application(bool imgui=false) : Elysium::Application(imgui)
    {
        m_TestMenu = new test::TestMenu(m_CurrentTest);
        m_CurrentTest = m_TestMenu;

        m_TestMenu->registerTest<test::Sandbox>("Sandbox");
        m_TestMenu->registerTest<test::ClearColor_Test>("Clear Color");
        m_TestMenu->registerTest<test::Texture2D_Test>("2D Texture");
        m_TestMenu->registerTest<test::ScreenSaver_Test>("Screen Saver");
        m_TestMenu->registerTest<test::BatchRendering_Test>("Batch Rendering");
        m_TestMenu->registerTest<test::DynamicBatchRendering_Test>("Dynamic Batch Rendering");

        m_ProjectionMatrix = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, -1.0f, 1.0f);
        m_ViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
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
            m_CurrentTest->onUpdate(0.0f);
            m_CurrentTest->onRender(m_ProjectionMatrix, m_ViewMatrix);
            ImGui::Begin("Test");
            if (m_CurrentTest != m_TestMenu && ImGui::Button("<--"))
            {
                delete m_CurrentTest;
                m_CurrentTest = m_TestMenu;
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
