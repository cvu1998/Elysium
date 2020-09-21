#pragma once

#include "Systems.h"

struct Cell
{
    Elysium::Vector2 Position = { 0.0f, 0.0f };
    Elysium::Vector4 Color = { 0.0f, 1.0f, 0.0f, 1.0f };
};

class CellArea
{
public:
    static constexpr size_t s_NumberOfCell = 1024 * 768;
    static constexpr size_t s_NumberOfCancerCell = 1024 * 768 / 4;

    std::array<Cell, s_NumberOfCell> Area;

private:
    inline Cell* getCell(int32_t row, int32_t column)
    {
        if (row >= 0 && row < 1024 && column >= 0 && column < 768)
            return &Area[(size_t)row * 3 + (size_t)column];
        return nullptr;
    }

public:
    CellArea(Elysium::Vector2 offset);

    void onUpdate(Elysium::Timestep ts);
};