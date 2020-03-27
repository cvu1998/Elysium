#include "DynamicBatchRendering_Test.h"

#include <imgui/imgui.h>

namespace test {

    DynamicBatchRendering_Test::DynamicBatchRendering_Test(): 
        m_Shader("res/shaders/batch_rendering.shader")
    {
        GL_ASSERT(glEnable(GL_BLEND));
        GL_ASSERT(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        Renderer2D::init();
        m_Shader.bind();

        int sampler[32];
        for (int i = 0; i < 32; i++)
            sampler[i] = i;
        m_Shader.setUniform1iv<32>("u_Textures", 32, sampler);

        m_Textures.reserve(2);
        m_Textures.emplace_back("res/texture/meadow.png");
        m_Textures.emplace_back("res/texture/Vader.png");
        m_Textures[0].bind();
        m_Textures[1].bind();
        std::cout << m_Textures[0].getRendererID() << std::endl;
        std::cout << m_Textures[1].getRendererID() << std::endl;
    }

    DynamicBatchRendering_Test::~DynamicBatchRendering_Test()
    {
        Renderer2D::shutdown();
        GL_ASSERT(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    }

    void DynamicBatchRendering_Test::onUpdate(float deltaTime)
    {
        Renderer2D::beginBatch();
        bool checked = false;
        glm::vec4 black = { 0.0f, 0.0f, 0.0f, 1.0f };
        glm::vec4 white = { 1.0f, 1.0f, 1.0f, 1.0f };

        for (float x = -4.0f; x < 4.0f; ++x)
        {
            for (float y = -3.0f; y < 4.0f; ++y)
            {
                if (checked) {
                    Renderer2D::drawQuad({ x, y }, { 1.0f, 1.0f }, black);
                    checked = false;
                }
                else {
                    Renderer2D::drawQuad({ x, y }, { 1.0f, 1.0f }, white);
                    checked = true;
                }
            }
        }
        glm::vec4 color = { 0.0f, 1.0f, 1.0f, 1.0f };
        Renderer2D::drawQuad({ -2.0f, 1.0f }, { 1.0f, 1.0f }, color);
        Renderer2D::drawQuad({ 1.0f, 1.0f }, { 1.0f, 1.0f }, color);

        Renderer2D::drawQuad({ 1.0f, -1.0f }, { 1.0f, 1.0f }, m_Textures[0].getRendererID());
        Renderer2D::drawQuad({ m_QuadPosition[0], m_QuadPosition[1] }, { 1.0f, 1.0f }, m_Textures[1].getRendererID());

        Renderer2D::endBatch();
    }

    void DynamicBatchRendering_Test::onRender(const glm::mat4& proj, const glm::mat4& view)
    {
        GL_ASSERT(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
        GL_ASSERT(glClear(GL_COLOR_BUFFER_BIT));

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 mvp = proj * view * model;
        m_Shader.setUniformMat4f("u_MVP", mvp);

        Renderer2D::flush();
    }

    void DynamicBatchRendering_Test::onImGuiRender()
    {
        ImGui::Begin("First Quad Controls");
        ImGui::DragFloat2("First Quad Position", m_QuadPosition, 0.1f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
}
