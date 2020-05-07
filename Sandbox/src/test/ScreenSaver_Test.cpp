#include "ScreenSaver_Test.h"

namespace test {

    ScreenSaver_Test::ScreenSaver_Test() :
        m_Shader("res/shaders/basic.shader"),
        m_Height(1.0f), m_Width(1.0f),
        r(0.0f), inc(true),
        m_Translation(1.0f), x(0.0f), y(0.0f), m_SignX(false), m_SignY(false)
    {
        float vertices[] = {
           -m_Height, -m_Width, 0.0f, 0.0f,    // 0
            m_Height, -m_Width, 1.0f, 0.0f,    // 1
            m_Height,  m_Width, 1.0f, 1.0f,    // 2
           -m_Height,  m_Width, 0.0f, 1.0f     // 3
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        m_vb = std::make_unique<VertexBuffer>(vertices, (unsigned int)(4 * 4 * sizeof(float)));
        VertexBufferLayout layout;
        layout.push<float>(2);
        layout.push<float>(2);
        m_va.addBuffer(*m_vb, layout);

        m_ib = std::make_unique<IndexBuffer>(indices, 6);

        m_Shader.bind();
        m_Shader.setUniform1i("u_UseTexture", 0);
    }

    ScreenSaver_Test::~ScreenSaver_Test()
    {
    }

    void ScreenSaver_Test::onUpdate(float deltaTime)
    {
        s_CameraController->onUpdate(deltaTime);

        /***CHECK WITHIN BORDERS***/
        if (!(x > -m_CornerWidth && x < m_CornerWidth) ||
            !(y > -m_CornerHeight && y < m_CornerHeight)) {
            if (!(x > -m_CornerWidth && x < m_CornerWidth) &&
                !(y > -m_CornerHeight && y < m_CornerHeight) && m_Case != OutOfBounds::XY) {
                m_SignX = !m_SignX;
                m_SignY = !m_SignY;
                m_Case = OutOfBounds::XY;
            }
            else if (!(x > -m_CornerWidth && x < m_CornerWidth) &&
                (y > -m_CornerHeight && y < m_CornerHeight) && m_Case != OutOfBounds::X_ONLY) {
                m_SignX = !m_SignX;
                m_Case = OutOfBounds::X_ONLY;
            }
            else if ((x > -m_CornerWidth && x < m_CornerWidth) &&
                !(y > -m_CornerHeight && y < m_CornerHeight) && m_Case != OutOfBounds::Y_ONLY) {
                m_SignY = !m_SignY;
                m_Case = OutOfBounds::Y_ONLY;
            }
        }
        else {
            m_Case = OutOfBounds::IN_BOUNDS;
        }
        /***CHECK WITHIN BORDERS***/
        /***BOUNCE LOGIC***/
        if (m_SignX && m_SignY) {
            x -= m_Translation * deltaTime;
            y -= m_Translation * deltaTime;
        }
        else if (m_SignX && !m_SignY) {
            x -= m_Translation * deltaTime;
            y += m_Translation * deltaTime;
        }
        else if (!m_SignX && m_SignY) {
            x += m_Translation * deltaTime;
            y -= m_Translation * deltaTime;
        }
        else {
            x -= -m_Translation * deltaTime;
            y -= -m_Translation * deltaTime;
        }
        /***BOUNCE LOGIC***/
        /***COLOR CHANGE***/
        if (r < 1.0f && inc) {
            r += 1.0f * deltaTime;
        }
        else if (r > 0 && !inc) {
            r -= 1.0f * deltaTime;
        }
        else {
            inc = !inc;
        }
        /***COLOR CHANGE***/
    }

    void ScreenSaver_Test::onRender()
    {
        m_Shader.bind();
        m_Shader.setUniform4f("u_Color", r, 0.0f, 1.0f, 1.0f);
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0));
            glm::mat4 mvp = s_CameraController->getCamera().getViewProjectionMatrix() * model;
            m_Shader.setUniformMat4f("u_ViewProjection", mvp);

            Renderer::Draw(m_va, *m_ib, m_Shader);
        }

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
            glm::mat4 mvp = s_CameraController->getCamera().getViewProjectionMatrix() * model;
            m_Shader.setUniformMat4f("u_ViewProjection", mvp);

            glBegin(GL_LINES);
                glVertex2f(-m_BoxWidth, -m_BoxHeight);
                glVertex2f(-m_BoxWidth,  m_BoxHeight);

                glVertex2f(-m_BoxWidth, -m_BoxHeight);
                glVertex2f( m_BoxWidth, -m_BoxHeight);

                glVertex2f( m_BoxWidth, m_BoxHeight);
                glVertex2f(-m_BoxWidth, m_BoxHeight);

                glVertex2f( m_BoxWidth,  m_BoxHeight);
                glVertex2f( m_BoxWidth, -m_BoxHeight);
            glEnd();
        }
    }

    void ScreenSaver_Test::onImGuiRender()
    {
        ImGui::SliderFloat("Translation Velocity", &m_Translation, 0.01f, 4.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
}
