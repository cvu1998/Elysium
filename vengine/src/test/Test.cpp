#include "Test.h"

#include <imgui/imgui.h>

namespace test {
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
}