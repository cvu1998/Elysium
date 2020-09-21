#include "CellGrowthScene.h"

CellGrowthScene::CellGrowthScene(unsigned int width, unsigned int height) :
    Elysium::Scene("Cell Growth"),
    m_CameraController((float)width / (float)height, 200.0f),
    m_Cells( {1024.0f * 0.5f, 768.0f * 0.5f} )
{
    m_CameraController.CameraTranslationSpeed = 200.0f;
    m_CameraController.CameraZoomSpeed = 5.0f;
}

CellGrowthScene::~CellGrowthScene()
{
}

void CellGrowthScene::onUpdate(Elysium::Timestep ts)
{
    m_CameraController.onUpdate(ts);
    Elysium::Renderer2D::beginScene(m_CameraController.getCamera());
    for (Cell& cell : m_Cells.Area)
    {
        Elysium::Renderer2D::drawPoint(cell.Position, cell.Color);
    }
    Elysium::Renderer2D::endScene();

    ImGui::Begin("Cell Growth");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    ImGui::Begin("Statistics");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Number of Draw Calls: %d", Elysium::Renderer2D::getStats().DrawCount);
    ImGui::End();

    Elysium::Renderer2D::resetStats();
}

void CellGrowthScene::onEvent(Elysium::Event& event)
{
    m_CameraController.onEvent(event);
}