#include "BatchRendering_Test.h"

namespace test {

    BatchRendering_Test::BatchRendering_Test(): 
        m_Shader("res/shaders/batch_rendering.shader"), 
        m_Translation(0.0f, 0.0f, 0.0f)
    {
        m_Textures.reserve(3);
        m_Textures.emplace_back();
        m_Textures.emplace_back("res/texture/meadow.png");
        m_Textures.emplace_back("res/texture/Vader.png");

        unsigned int white = m_Textures[0].getRendererID();
        unsigned int meadow = m_Textures[1].getRendererID();
        unsigned int vader = m_Textures[2].getRendererID();

        float vertices[] = {
/*         x      y     R     G     B   Alpha TextureCoord TextureID     */
         -1.5f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, float(meadow),
         -0.5f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, float(meadow),
         -0.5f,  0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, float(meadow),
         -1.5f,  0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, float(meadow),

          0.5f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, float(vader),
          1.5f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, float(vader),
          1.5f,  0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, float(vader),
          0.5f,  0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, float(vader),

         -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, float(white),
          0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, float(white),
          0.5f,  1.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, float(white),
         -0.5f,  1.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, float(white)
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

        int sampler[32];
        for (int i = 0; i < 32; i++)
            sampler[i] = i;
        m_Shader.setUniform1iv<32>("u_Textures", 32, sampler);

        m_Textures[0].bind(white);
        m_Textures[1].bind(meadow);
        m_Textures[2].bind(vader);
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

        glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Translation);
        glm::mat4 mvp = proj * view * model;
        m_Shader.setUniformMat4f("u_MVP", mvp);

        Renderer::draw(m_va, *m_ib, m_Shader);
    }

    void BatchRendering_Test::onImGuiRender()
    {
        ImGui::SliderFloat3("Translation", &m_Translation.x, -3.0f, 3.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
}
