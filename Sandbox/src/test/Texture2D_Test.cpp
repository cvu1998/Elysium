#include "Texture2D_Test.h"

namespace test {

    Texture2D_Test::Texture2D_Test(): m_TranslationA(-1.0f, 0.0f, 0.0f), m_TranslationB(1.0f, 0.0f, 0.0f),
        m_Shader("res/shaders/basic.shader"), m_Texture("res/texture/meadow.png")
    {
        float vertices[] = {
         -0.5f, -0.5f, 0.0f, 0.0f,    // 0
          0.5f, -0.5f, 1.0f, 0.0f,    // 1
          0.5f,  0.5f, 1.0f, 1.0f,    // 2

         -0.5f,  0.5f, 0.0f, 1.0f,    // 3
          0.5f,  1.0f, 1.0f, 1.0f,    // 4
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0,
            2, 4, 3
        };

        m_vb = std::make_unique<VertexBuffer>(vertices, 5 * 4 * sizeof(float));

        VertexBufferLayout layout;
        layout.push<float>(2);
        layout.push<float>(2);
        m_va.addBuffer(*m_vb, layout);

        m_ib = std::make_unique<IndexBuffer>(indices, 9);
    }

    Texture2D_Test::~Texture2D_Test()
    {
    }

    void Texture2D_Test::onUpdate(float deltaTime)
    {
        s_CameraController->onUpdate(deltaTime);
    }

    void Texture2D_Test::onRender()
    {
        m_Shader.bind();
        m_Shader.setUniform1i("u_UseTexture", 1);
        m_Texture.bind();
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
            glm::mat4 mvp = s_CameraController->getCamera().getViewProjectionMatrix() * model;
            m_Shader.setUniformMat4f("u_ViewProjection", mvp);

            Renderer::Draw(m_va, *m_ib, m_Shader);
        }

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);
            glm::mat4 mvp = s_CameraController->getCamera().getViewProjectionMatrix() * model;
            m_Shader.setUniformMat4f("u_ViewProjection", mvp);

            Renderer::Draw(m_va, *m_ib, m_Shader);
        }
    }

    void Texture2D_Test::onImGuiRender()
    {
        ImGui::SliderFloat3("Translation A", &m_TranslationA.x, -3.0f, 3.0f);
        ImGui::SliderFloat3("Translation B", &m_TranslationB.x, -3.0f, 3.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
}
