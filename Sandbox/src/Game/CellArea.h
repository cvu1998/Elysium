#pragma once

#include "Systems.h"

class CellArea
{
public:
    //static constexpr size_t NumberOfCell_X = 9;
    //static constexpr size_t NumberOfCell_Y = 9;
    static constexpr size_t NumberOfCell_X = 1024;
    static constexpr size_t NumberOfCell_Y = 768;

    static constexpr size_t NumberOfCells = NumberOfCell_X * NumberOfCell_Y;

private:
    enum class CellType
    {
        CANCER = 0,
        HEALTHY = 1,
        MEDECINE = 2
    };

    //static constexpr size_t s_NumberOfThreads = 3;
    static constexpr size_t s_NumberOfThreads = 4;
    static constexpr size_t s_NumberOfCellPerPartition = NumberOfCells / s_NumberOfThreads;
    static constexpr size_t s_NumberOfCellPerPartition_Y = NumberOfCell_Y / s_NumberOfThreads;

    float m_CurrentTime = 0.0f;

public:
    std::array<Elysium::Vector2, NumberOfCells> Positions;
    std::array<Elysium::Vector4, NumberOfCells> Colors;
    std::array<CellType, NumberOfCells> Types = { CellType::HEALTHY };
    std::array<std::vector<int32_t>, NumberOfCells> Neighbors;

    unsigned int NumberOfCancerCell = 0;
    unsigned int NumberOfHealthyCell = 0;
    unsigned int NumberOfMedecineCell = 0;

private:
    void setNeighbor(size_t index, int32_t row, int32_t column, int32_t bound_x, int32_t bound_y);

    void updateCellsInPartition(size_t min);
    void updateCell(CellType* partition, size_t partitionIndex, size_t index);

public:
    CellArea(Elysium::Vector2 offset);

    void onUpdate(Elysium::Timestep ts);
};