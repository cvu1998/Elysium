#pragma once

#include "Game/CellArea.h"

class CellGrowthScene : public Elysium::Scene
{
private:
    bool m_Pause = true;
    float m_Cooldown = 0.0f;
    unsigned int m_WindowWidth;
    unsigned int m_WindowHeight;

    Elysium::OrthographicCameraController m_CameraController;

    CellArea m_Cells;

private:
    Elysium::Vector2 getCursorPosition();

public:
    CellGrowthScene(unsigned int width, unsigned int height);
    ~CellGrowthScene();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};