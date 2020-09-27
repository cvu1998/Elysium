#include "CellGrowthScene.h"

CellGrowthScene::CellGrowthScene(unsigned int width, unsigned int height) :
    Elysium::Scene("Cell Growth"),
    m_CameraController((float)width / (float)height, 200.0f),
    m_Cells({ (float)CellArea::NumberOfCell_X * 0.5f, CellArea::NumberOfCell_Y * 0.5f })
{
    m_CameraController.CameraTranslationSpeed = 200.0f;
    m_CameraController.CameraZoomSpeed = 5.0f;
}

CellGrowthScene::~CellGrowthScene()
{
}

void CellGrowthScene::onUpdate(Elysium::Timestep ts)
{
    if (!m_Pause)
    {
        //m_Pause = true;
        m_Cells.onUpdate(ts);
    }

    m_CameraController.onUpdate(ts);
    Elysium::Renderer2D::beginScene(m_CameraController.getCamera());
    for (size_t i = 0; i < CellArea::NumberOfCells; i++)
    {
        Elysium::Renderer2D::drawPoint(m_Cells.Positions[i], m_Cells.Colors[i]);
    }
    Elysium::Renderer2D::endScene();

    ImGui::Begin("Cell Growth");
    ImGui::Checkbox("Pause Scene", &m_Pause);
    ImGui::Text("Number of Cells: %d", CellArea::NumberOfCells);
    ImGui::Text("Number of Cancer Cells: %d", m_Cells.NumberOfCancerCell);
    ImGui::Text("Number of Healthy Cells: %d", m_Cells.NumberOfHealthyCell);
    ImGui::Text("Number of Medecine Cells: %d", m_Cells.NumberOfMedecineCell);
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

bool CellGrowthScene::onWindowResizeEvent(Elysium::WindowResizeEvent& event)
{
    return false;
}