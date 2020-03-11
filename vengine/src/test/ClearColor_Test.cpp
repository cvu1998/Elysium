#include "ClearColor_Test.h"

#include <imgui/imgui.h>

#include "Utility.h"

namespace test {

    ClearColor::ClearColor() : m_Color {0.25f, 0.25f, 0.4f, 0.0f}
    {
    }

    ClearColor::~ClearColor()
    {
    }

    void ClearColor::onUpdate(float deltaTime)
    {
    }

    void ClearColor::onRender()
    {
        GL_ASSERT(glClearColor(m_Color[0], m_Color[1], m_Color[2] , m_Color[3]));
        GL_ASSERT(glClear(GL_COLOR_BUFFER_BIT))
    }

    void ClearColor::onImGuiRender()
    {
        ImGui::ColorEdit4("Clear Color", m_Color);
    }
}
