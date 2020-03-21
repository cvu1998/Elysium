#include "BatchRendering_Test.h"

#include <imgui/imgui.h>

namespace test {

    BatchRendering_Test::BatchRendering_Test(): 
        m_Shader("res/shaders/batch_rendering.shader"), 
        m_Translation(0.0f, 0.0f, 0.0f)
    {
        float vertices[] = {
/*         x      y     R     G     B   Alpha TextureCoord TextureID     */
         -1.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
         -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
         -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
         -1.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

          0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
          1.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
          1.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
          0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,

         -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
          0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f,
          0.5f,  1.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
         -0.5f,  1.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, -1.0f
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0,
            4, 5, 6,
            6, 7, 4,
            8, 9, 10,
            10, 11, 8
        };

        GL_ASSERT(glEnable(GL_BLEND));
        GL_ASSERT(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        m_vb = std::make_unique<VertexBuffer>(vertices, 12 * 9 * sizeof(float));

        VertexBufferLayout layout;
        layout.push<float>(2);
        layout.push<float>(4);
        layout.push<float>(2);
        layout.push<float>(1);
        m_va.addBuffer(*m_vb, layout);

        m_ib = std::make_unique<IndexBuffer>(indices, 18);

        m_Shader.bind();
        m_Textures.reserve(2);
        m_Textures.emplace_back("res/texture/meadow.png");
        m_Textures.emplace_back("res/texture/Vader.png");

        int sampler[2] = { 0, 1 };
        m_Shader.setUniform1iv<2>("u_Textures", 2, sampler);

        m_Textures[0].bind(0);
        m_Textures[1].bind(1);
        //glBindTextureUnit(0, m_Textures[0].getRendererID());
        //glBindTextureUnit(1, m_Textures[1].getRendererID());
    }

    BatchRendering_Test::~BatchRendering_Test()
    {
        GL_ASSERT(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    }

    void BatchRendering_Test::onUpdate(float deltaTime)
    {
    }

    void BatchRendering_Test::onRender(const glm::mat4& proj, const glm::mat4& view)
    {
        GL_ASSERT(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
        GL_ASSERT(glClear(GL_COLOR_BUFFER_BIT));

        Renderer& s_Renderer = Renderer::getInstance();
        glm::mat4 modelT = glm::translate(glm::mat4(1.0f), m_Translation);
        glm::mat4 mvpT = proj * view * modelT;
        m_Shader.setUniformMat4f("u_MVP", mvpT);
        s_Renderer.draw(m_va, *m_ib, m_Shader);
    }

    void BatchRendering_Test::onImGuiRender()
    {
        ImGui::SliderFloat3("Translation", &m_Translation.x, -3.0f, 3.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
}
