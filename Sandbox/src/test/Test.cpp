#include "Test.h"

namespace test 
{
    Elysium::OrthographicCameraController* test::Test::s_CameraController = nullptr;

    TestMenu::TestMenu(Test*& currentTestPointer) : m_CurrentTest(currentTestPointer)
    {
    }

    void TestMenu::onImGuiRender()
    {
        for (auto& test : m_Tests)
        {
            if (ImGui::Button(test.first))
            {
                m_CurrentTest = test.second();
            }
        }
    }

    void Test::onEvent(Elysium::Event& event)
    {
        s_CameraController->onEvent(event);
    }

    void Test::destoryCamera()
    {
        delete s_CameraController;
    }
}