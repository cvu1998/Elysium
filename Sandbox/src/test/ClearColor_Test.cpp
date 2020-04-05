#include "ClearColor_Test.h"

namespace test {

    ClearColor_Test::ClearColor_Test() : m_Color { (20.0f / 255.0f), (220.0f / 255.0f), 1.0f, 0.0f }
    {
    }

    ClearColor_Test::~ClearColor_Test()
    {
        GL_ASSERT(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    }

    void ClearColor_Test::onUpdate(float deltaTime)
    {
    }

    void ClearColor_Test::onRender(const glm::mat4& proj, const glm::mat4& view)
    {
        GL_ASSERT(glClearColor(m_Color[0], m_Color[1], m_Color[2] , m_Color[3]));
        GL_ASSERT(glClear(GL_COLOR_BUFFER_BIT))
    }

    void ClearColor_Test::onImGuiRender()
    {
        ImGui::ColorEdit4("Clear Color", m_Color);
    }
}
