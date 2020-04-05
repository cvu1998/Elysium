#include "test/ClearColor_Test.h"
#include "test/Texture2D_Test.h"
#include "test/ScreenSaver_Test.h"
#include "test/BatchRendering_Test.h"
#include "test/DynamicBatchRendering_Test.h"
#include "test/Sandbox.h"

class Application : public Elysium::Application
{
private:
    test::Test* currentTest;
    test::TestMenu* testMenu;

    glm::mat4 view;
    glm::mat4 proj;

public:

    Application(bool imgui=false) : Elysium::Application(imgui)
    {
        testMenu = new test::TestMenu(currentTest);
        currentTest = testMenu;

        testMenu->registerTest<test::Sandbox>("Sandbox");
        testMenu->registerTest<test::ClearColor_Test>("Clear Color");
        testMenu->registerTest<test::Texture2D_Test>("2D Texture");
        testMenu->registerTest<test::ScreenSaver_Test>("Screen Saver");
        testMenu->registerTest<test::BatchRendering_Test>("Batch Rendering");
        testMenu->registerTest<test::DynamicBatchRendering_Test>("Dynamic Batch Rendering");

        view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        proj = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, -1.0f, 1.0f);
    }

    ~Application()
    {
        delete currentTest;
        if (currentTest != testMenu)
            delete testMenu;
    }

    void ApplicationLogic() override
    {
        if (currentTest)
        {
            currentTest->onUpdate(0.0f);
            currentTest->onRender(proj, view);
            ImGui::Begin("Test");
            if (currentTest != testMenu && ImGui::Button("<--"))
            {
                delete currentTest;
                currentTest = testMenu;
            }
            currentTest->onImGuiRender();
            ImGui::End();
        }
    }
};

int main(void)
{
    Application* application = new Application(true);
    application->Run();
    delete application;
    return 0;
}
