#include "DynamicBatchRendering_Test.h"

namespace test {

    DynamicBatchRendering_Test::DynamicBatchRendering_Test()
    {
        m_Textures.reserve(2);
        m_Textures.emplace_back("res/texture/meadow.png");
        m_Textures.emplace_back("res/texture/Vader.png");
    }

    DynamicBatchRendering_Test::~DynamicBatchRendering_Test()
    {
    }

    void DynamicBatchRendering_Test::onUpdate(float deltaTime)
    {
        s_CameraController->onUpdate(deltaTime);

        Elysium::Renderer2D::beginScene(s_CameraController->getCamera());

        for (float x = -4.0f; x < 4.0f; x += 0.1f)
        {
            for (float y = -3.0f; y < 3.0f; y += 0.1f)
            {
                glm::vec4 gradient = { (x + 4.0f) / 8.0f, (y + 3.0f) / 12.0f, 1.0f, 1.0f };
                Elysium::Renderer2D::drawQuadWithRotation({ x + 0.025f , y + 0.025f }, { 0.05f, 0.05f }, glm::radians(m_RotationSpeed), gradient);
            }
        }

        glm::vec4 black = { 0.0f, 0.0f, 0.0f, 1.0f };
        glm::vec4 white = { 1.0f, 1.0f, 1.0f, 1.0f };

        for (int x = -2; x < 2; x++)
        {
            for (int y = -2; y < 2; y++)
            {
                glm::vec4 color = (x + y) % 2 == 0 ? white : black;
                Elysium::Renderer2D::drawQuad({ x + 0.5f , y + 0.5f }, { 1.0f, 1.0f }, color);
            }
        }
        glm::vec4 color = { 0.0f, 1.0f, 1.0f, 1.0f };
        Elysium::Renderer2D::drawQuadWithRotation({ -1.5f, 1.5f }, { 1.0f, 1.0f }, glm::radians(m_RotationSpeed), color);
        Elysium::Renderer2D::drawQuad({ 1.5f, 1.5f }, { 1.0f, 1.0f }, color);

        Elysium::Renderer2D::drawQuadWithRotation({ 1.5f, -1.5f }, { 1.0f, 1.0f }, glm::radians(m_RotationSpeed), m_Textures[1].getTextureData());
        Elysium::Renderer2D::drawQuad({ m_QuadPosition[0], m_QuadPosition[1] }, { 1.0f, 1.0f }, m_Textures[0].getTextureData(),
            { m_QuadColor[0], m_QuadColor[1], m_QuadColor[2], m_QuadColor[3] });

        Elysium::Renderer2D::drawQuadWithRotation({ 0.0f, 0.0f }, { 2.0f, 1.0f }, glm::radians(m_RotationSpeed), color);

        m_RotationSpeed += 120.0f * deltaTime;
        Elysium::Renderer2D::endScene();
    }

    void DynamicBatchRendering_Test::onRender()
    {   
    }

    void DynamicBatchRendering_Test::onImGuiRender()
    {
        ImGui::Begin("First Quad Controls");
        ImGui::DragFloat2("First Quad Position", m_QuadPosition, 0.1f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Number of Draw Calls: %d", Elysium::Renderer2D::getStats().DrawCount);
        ImGui::Text("Number of Quads: %d", Elysium::Renderer2D::getStats().QuadCount);
        ImGui::End();

        Elysium::Renderer2D::resetStats();
    }
}
