#include "CellGrowthScene.h"

CellGrowthScene::CellGrowthScene(unsigned int width, unsigned int height) :
    Elysium::Scene("Cell Growth"),
    m_WindowWidth(width),
    m_WindowHeight(height),
    m_CameraController((float)width / (float)height, 500.0f),
    m_Cells({ (float)CellArea::NumberOfCell_X * 0.5f, CellArea::NumberOfCell_Y * 0.5f })
{
    m_CameraController.CameraTranslationSpeed = 200.0f;
    m_CameraController.CameraZoomSpeed = 10.0f;
}

CellGrowthScene::~CellGrowthScene()
{
}

void CellGrowthScene::onUpdate(Elysium::Timestep ts)
{
    Elysium::Vector2 cursorPosition = getCursorPosition();

    if (!m_Pause)
    {
        m_Cooldown += ts;
        if (Elysium::Input::isMouseButtonPressed(ELY_MOUSE_BUTTON_1) && m_Cooldown >= 0.0f)
        {
            m_Cells.injectMedecine(cursorPosition);
            m_Cooldown = -0.1f;
        }

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
    ImGui::Text("Number of Cells Accounted: %d", m_Cells.NumberOfCancerCells + m_Cells.NumberOfHealthyCells + m_Cells.NumberOfMedecineCells);
    ImGui::Text("Number of Cancer Cells: %d", m_Cells.NumberOfCancerCells);
    ImGui::Text("Number of Healthy Cells: %d", m_Cells.NumberOfHealthyCells);
    ImGui::Text("Number of Medecine Cells: %d", m_Cells.NumberOfMedecineCells);
    ImGui::Text("Cell Index: %d", m_Cells.getIndex(cursorPosition));
    ImGui::End();

    ImGui::Begin("Statistics");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Number of Draw Calls: %d", Elysium::Renderer2D::getStats().DrawCount);
    ImGui::End();

    Elysium::Renderer2D::resetStats();
}

Elysium::Vector2 CellGrowthScene::getCursorPosition()
{
    auto position = Elysium::Input::getMousePosition();

    return  m_CameraController.getCamera().getScreenToWorldPosition(m_WindowWidth, m_WindowHeight, position);
}

void CellGrowthScene::onEvent(Elysium::Event& event)
{
    m_CameraController.onEvent(event);

    Elysium::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Elysium::WindowResizeEvent>(BIND_EVENT_FUNCTION(CellGrowthScene::onWindowResizeEvent));
}

bool CellGrowthScene::onWindowResizeEvent(Elysium::WindowResizeEvent& event)
{
    m_WindowWidth = event.getWidth();
    m_WindowHeight = event.getHeight();
    return false;
}