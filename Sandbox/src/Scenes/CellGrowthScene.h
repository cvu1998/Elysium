#pragma once

#include "Game/CellArea.h"

class CellGrowthScene : public Elysium::Scene
{
private:
    float m_AspectRatio = 0.0f;

    Elysium::OrthographicCameraController m_CameraController;

    CellArea m_Cells;

public:
    CellGrowthScene(unsigned int width, unsigned int height);
    ~CellGrowthScene();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);

};