#include "CellArea.h"

CellArea::CellArea(Elysium::Vector2 offset)
{
    float cellSize = 2.0f;

    bool firstPass = true;
    for (size_t i = 0; i < s_NumberOfCell; i++)
    {
        if (firstPass)
            Area[i].Position = { ((float)(i % 1024) - offset.x) + cellSize, ((float)(i / 1024) - offset.y) + cellSize };

        if (Random::Float() < 0.30f)
        {
            Area[i].Color = { 1.0f, 0.0f, 0.0f, 1.0f };
        }
    }

    Elysium::Renderer2D::setPointSize(cellSize);
}

void CellArea::onUpdate(Elysium::Timestep ts)
{
}