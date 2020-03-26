#include "DynamicBatchRendering_Test.h"

#include <imgui/imgui.h>

namespace test {

    static const size_t MaxQuadCount = 1000;
    static const size_t MaxVertexCount = MaxQuadCount * 4;
    static const size_t MaxIndexCount = MaxQuadCount * 6;

    DynamicBatchRendering_Test::DynamicBatchRendering_Test(): 
        m_vb(MaxVertexCount),
        m_Shader("res/shaders/batch_rendering.shader")
    {
        GL_ASSERT(glEnable(GL_BLEND));
        GL_ASSERT(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        VertexBufferLayout layout;
        layout.push<Vertex>(2);
        layout.push<Vertex>(4);
        layout.push<Vertex>(2);
        layout.push<Vertex>(1);
        m_va.addBuffer(m_vb, layout);

        unsigned int offset = 0;
        unsigned int indices[MaxIndexCount];
        for (size_t i = 0; i < MaxIndexCount; i += 6)
        {
            indices[i + 0] = 0 + offset;
            indices[i + 1] = 1 + offset;
            indices[i + 2] = 2 + offset;
            indices[i + 3] = 2 + offset;
            indices[i + 4] = 3 + offset;
            indices[i + 5] = 0 + offset;

            offset += 4;
        }

        /*m_ib = std::make_unique<IndexBuffer>(m_MaxIndexCount);*/
        m_ib = std::make_unique<IndexBuffer>(indices, MaxIndexCount);

        m_Shader.bind();
        m_Textures.reserve(2);
        m_Textures.emplace_back("res/texture/meadow.png");
        m_Textures.emplace_back("res/texture/Vader.png");

        int sampler[2] = { 0, 1 };
        m_Shader.setUniform1iv<2>("u_Textures", 2, sampler);

        m_Textures[0].bind(0);
        m_Textures[1].bind(1);
    }

    DynamicBatchRendering_Test::~DynamicBatchRendering_Test()
    {
        GL_ASSERT(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    }

    void DynamicBatchRendering_Test::onUpdate(float deltaTime)
    {
        m_IndexCount = 0;
        std::array<Vertex, MaxVertexCount> vertices;
        Vertex* buffer = vertices.data();

        bool checked = false;
        glm::vec4 black = { 0.0f, 0.0f, 0.0f, 1.0f };
        glm::vec4 white = { 1.0f, 1.0f, 1.0f, 1.0f };

        for (float x = -4.0f; x < 4.0f; ++x)
        {   
            for (float y = -3.0f; y < 4.0f; ++y)
            {
                if (checked) {
                    buffer = Shape::createSquare(buffer, { x, y }, 1.0f, black, -1.0f);
                    checked = false;
                }
                else {
                    buffer = Shape::createSquare(buffer, { x, y }, 1.0f, white, -1.0f);
                    checked = true;
                }
                m_IndexCount += 6;
            }
        }
        
        glm::vec4 color = { 0.0f, 1.0f, 1.0f, 1.0f };
        buffer = Shape::createSquare(buffer, { m_QuadPosition[0], m_QuadPosition[1] }, 1.0f, color,  0.0f);
        buffer = Shape::createSquare(buffer, {  1.0f, -1.0f }, 1.0f, color,  1.0f);
        buffer = Shape::createSquare(buffer, { -2.0f,  1.0f }, 1.0f, color, -1.0f);
        buffer = Shape::createSquare(buffer, {  1.0f,  1.0f }, 1.0f, { 1.0f, 0.0f, 1.0f, 1.0f }, -1.0f);
        m_IndexCount += 24;

        GL_ASSERT(glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data()));
        /*
        m_ib->bind();
        unsigned int indices[] = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4,
            8, 9, 10, 10, 11, 8,
            12, 13, 14, 14, 15, 12,
            9, 10, 12, 12, 15, 10
        };

        GL_ASSERT(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices));
        */
    }

    void DynamicBatchRendering_Test::onRender(const glm::mat4& proj, const glm::mat4& view)
    {
        GL_ASSERT(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
        GL_ASSERT(glClear(GL_COLOR_BUFFER_BIT));

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 mvp = proj * view * model;
        m_Shader.setUniformMat4f("u_MVP", mvp);
        m_Shader.bind();
        
        Renderer::draw(m_va, *m_ib, m_Shader, m_IndexCount);
    }

    void DynamicBatchRendering_Test::onImGuiRender()
    {
        ImGui::Begin("First Quad Controls");
        ImGui::DragFloat2("First Quad Position", m_QuadPosition, 0.1f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
}
