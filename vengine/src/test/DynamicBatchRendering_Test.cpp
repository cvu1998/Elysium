#include "DynamicBatchRendering_Test.h"

#include <imgui/imgui.h>

namespace test {

    DynamicBatchRendering_Test::DynamicBatchRendering_Test(): 
        m_vb(1000), m_ib(100),
        m_Shader("res/shaders/batch_rendering.shader"),
        m_Translation(0.0f, 0.0f, 0.0f)
    {
        GL_ASSERT(glEnable(GL_BLEND));
        GL_ASSERT(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        VertexBufferLayout layout;
        layout.push<Vertex>(2);
        layout.push<Vertex>(4);
        layout.push<Vertex>(2);
        layout.push<Vertex>(1);
        m_va.addBuffer(m_vb, layout);

       /*
       glGenBuffers(1, &m_VertexRendererID);
       glBindBuffer(GL_ARRAY_BUFFER, m_VertexRendererID);
       glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 1000, nullptr, GL_DYNAMIC_DRAW);

       glGenBuffers(1, &m_IndexRendererID);
       glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexRendererID);
       glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof(GLuint), indices, GL_STATIC_DRAW);

       glEnableVertexAttribArray(0);
       glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

       glEnableVertexAttribArray(1);
       glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

       glEnableVertexAttribArray(2);
       glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, textureCoord));

       glEnableVertexAttribArray(3);
       glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, textureID));
       */

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

    DynamicBatchRendering_Test::~DynamicBatchRendering_Test()
    {
        GL_ASSERT(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    }

    void DynamicBatchRendering_Test::onUpdate(float deltaTime)
    {
        m_vb.bind();
        Vec4 color = { 0.0f, 1.0f, 1.0f, 1.0f };
        auto sq0 = Shape::createSquare({ m_QuadPosition[0], m_QuadPosition[1] }, 1.0f, color,  0.0f);
        auto sq1 = Shape::createSquare({  0.5f, -1.0f }, 1.0f, color,  1.0f);
        auto sq2 = Shape::createSquare({ -1.5f,  1.0f }, 1.0f, color, -1.0f);
        auto sq3 = Shape::createSquare({  0.5f,  1.0f }, 1.0f, { 1.0f, 0.0f, 1.0f, 1.0f }, -1.0f);

        Vertex vertices[16];
        void* destination = vertices;
        memcpy(destination, sq0.data(), sq0.size() * sizeof(Vertex));
        destination = vertices + sq0.size();
        memcpy(destination, sq1.data(), sq1.size() * sizeof(Vertex));
        destination = vertices + sq0.size() + sq1.size();
        memcpy(destination, sq2.data(), sq2.size() * sizeof(Vertex));
        destination = vertices + sq0.size() + sq1.size() + sq2.size();
        memcpy(destination, sq3.data(), sq3.size() * sizeof(Vertex));

        GL_ASSERT(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices));

        m_ib.bind();
        unsigned int indices[] = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4,
            8, 9, 10, 10, 11, 8,
            12, 13, 14, 14, 15, 12,
            1, 2, 4, 4, 7, 2
        };

        GL_ASSERT(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices));
    }

    void DynamicBatchRendering_Test::onRender(const glm::mat4& proj, const glm::mat4& view)
    {
        GL_ASSERT(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
        GL_ASSERT(glClear(GL_COLOR_BUFFER_BIT));

        Renderer& s_Renderer = Renderer::getInstance();
        glm::mat4 modelT = glm::translate(glm::mat4(1.0f), m_Translation);
        glm::mat4 mvpT = proj * view * modelT;
        m_Shader.setUniformMat4f("u_MVP", mvpT);
        m_Shader.bind();
        s_Renderer.draw(m_va, m_ib, m_Shader);
        //GL_ASSERT(glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, nullptr));
    }

    void DynamicBatchRendering_Test::onImGuiRender()
    {
        ImGui::SliderFloat3("Translation", &m_Translation.x, -3.0f, 3.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::Begin("First Quad Controls");
        ImGui::DragFloat2("Quad Position", m_QuadPosition, 0.1f);
        ImGui::End();
    }
}
