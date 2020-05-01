#include "ClearColor_Test.h"

namespace test {

    ClearColor_Test::ClearColor_Test() : m_Color { (20.0f / 255.0f), (220.0f / 255.0f), 1.0f, 0.0f }
    {
    }

    ClearColor_Test::~ClearColor_Test()
    {
    }

    void ClearColor_Test::onUpdate(float deltaTime)
    {
        s_CameraController->onUpdate(deltaTime);
    }

    void ClearColor_Test::onRender()
    {
        GL_ASSERT(glClearColor(m_Color[0], m_Color[1], m_Color[2] , m_Color[3]));
        GL_ASSERT(glClear(GL_COLOR_BUFFER_BIT))
    }

    void ClearColor_Test::onImGuiRender()
    {
        ImGui::ColorEdit4("Clear Color", m_Color);
    }
}
