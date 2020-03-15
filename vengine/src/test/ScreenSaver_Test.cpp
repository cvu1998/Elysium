#include "ScreenSaver_Test.h"

namespace test {

    ScreenSaver::ScreenSaver() : r(0.0f), inc(true),
        x(0.0f), y(0.0f), m_SignX(false), m_SignY(false)
    {
        unsigned int indices[] = {
              5, 6, 7,
              7, 8, 5
        };
        m_ib = new IndexBuffer(indices, 6);

        m_Shader = new Shader("res/shaders/basic.shader");
        m_Shader->bind();
        m_Shader->setUniform1i("u_UseTexture", 0);
    }

    ScreenSaver::~ScreenSaver()
    {
        delete m_ib;
        delete m_Shader;
    }

    void ScreenSaver::destroyScreenSaver()
    {
        delete s_Instance;
    }

    void ScreenSaver::setScreenSaver(const VertexBuffer& vb)
    {
        m_Layout.push<float>(2);
        m_Layout.push<float>(2);
        m_va.addBuffer(vb, m_Layout);
    }

    void ScreenSaver::updateScreenSaver(Renderer& renderer, const glm::mat4& proj, const glm::mat4& view)
    {
        float translation = 0.01f;
        /***CHECK WITHIN BORDERS***/
        if (!(x > -3.0f && x < 3.0f) &&
            !(y > -2.0f && y < 2.0f)) {
            m_SignX = !m_SignX;
            m_SignY = !m_SignY;
        }
        else if (!(x > -3.0f && x < 3.0f) &&
            (y > -2.0f && y < 2.0f)) {
            m_SignX = !m_SignX;
        }
        else if ((x > -3.0f && x < 3.0f) &&
            !(y > -2.0f && y < 2.0f)) {
            m_SignY = !m_SignY;
        }
        /***CHECK WITHIN BORDERS***/
        /***BOUNCE LOGIC***/
        if (m_SignX && m_SignY) {
            x += translation;
            y += translation;
        }
        else if (m_SignX && !m_SignY) {
            x += translation;
            y -= translation;
        }
        else if (!m_SignX && m_SignY) {
            x -= translation;
            y += translation;
        }
        else {
            x += -translation;
            y += -translation;
        }
        /***BOUNCE LOGIC***/
        /***COLOR CHANGE***/
        if (r < 1.0f && inc) {
            r += 0.01f;
        }
        else if (r > 0 && !inc) {
            r -= 0.01f;
        }
        else {
            inc = !inc;
        }
        /***COLOR CHANGE***/

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0));
        glm::mat4 mvp = proj * view * model;

        m_Shader->bind();
        m_Shader->setUniform4f("u_Color", r, 0.0f, 1.0f, 1.0f);
        m_Shader->setUniformMat4f("u_MVP", mvp);

        renderer.draw(m_va, *m_ib, *m_Shader);
    }

    void ScreenSaver::unBind() const
    {
        m_va.unBind();
        m_Shader->unBind();
        m_ib->unBind();
    }
}
