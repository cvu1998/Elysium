#include "CellArea.h"

#include <set>

CellArea::CellArea(Elysium::Vector2 offset)
{
    float cellSize = 5.0f;

    float percentage = Random::Float();
    constexpr size_t MinimumNumberOfCancerCell = NumberOfCells / 4;
    NumberOfCancerCell = (unsigned int)(percentage * MinimumNumberOfCancerCell) + MinimumNumberOfCancerCell;
    unsigned int counter = 0;
    std::set<size_t>  Indexes;
    while (counter < NumberOfCancerCell)
    {
        size_t index = (size_t)Random::Integer(0, NumberOfCells);
        while (Indexes.find(index) != Indexes.end())
            index = (size_t)Random::Integer(0, NumberOfCells);
        Indexes.insert(index);
        counter++;
    }

    for (size_t i = 0; i < NumberOfCells; i++)
    {
        size_t partitionIndex = i % s_NumberOfCellPerPartition;
        int32_t partition_x = (int32_t)(partitionIndex % NumberOfCell_X);
        int32_t partition_y = (int32_t)(partitionIndex / NumberOfCell_X);
        setNeighbor(i, partition_x, partition_y, NumberOfCell_X, s_NumberOfCellPerPartition_Y);

        Positions[i] = { ((float)(i % NumberOfCell_X) - offset.x) + cellSize, ((float)(i / NumberOfCell_X) - offset.y) + cellSize };
        if (Indexes.find(i) != Indexes.end())
        {
            Colors[i] = { 1.0f, 0.0f, 0.0f, 1.0f };
            Types[i] = CellType::CANCER;
        }
        else
        {
            Colors[i] = { 0.0f, 1.0f, 0.0f, 1.0f };
            Types[i] = CellType::HEALTHY;
            NumberOfHealthyCell++;
        }
    }
    /*
    for (size_t i = 0; i < NumberOfCells; i++)
    {
        size_t partitionIndex = i % s_NumberOfCellPerPartition;
        int32_t partition_x = (int32_t)(partitionIndex % NumberOfCell_X);
        int32_t partition_y = (int32_t)(partitionIndex / NumberOfCell_X);
        setNeighbor(i, partition_x, partition_y, NumberOfCell_X, s_NumberOfCellPerPartition_Y);

        Positions[i] = { ((float)(i % NumberOfCell_X) - offset.x) + cellSize, ((float)(i / NumberOfCell_X) - offset.y) + cellSize };;
        if (i == 4 + ((i / NumberOfCell_Y) * NumberOfCell_X))
        {
            Colors[i] = { 1.0f, 0.0f, 0.0f, 1.0f };
            Types[i] = CellType::CANCER;
            NumberOfHealthyCell++;
        }
        else
        {
            Colors[i] = { 0.0f, 1.0f, 0.0f, 1.0f };
            Types[i] = CellType::HEALTHY;
            NumberOfCancerCell++;
        }
    }
    */

    Elysium::Renderer2D::setPointSize(cellSize);

    const unsigned int processor_count = std::thread::hardware_concurrency();
    ELY_INFO("Number of cell per partition: {0}", s_NumberOfCellPerPartition);
    ELY_INFO("Number of threads: {0}", processor_count);
}

void CellArea::onUpdate(Elysium::Timestep ts)
{
    m_CurrentTime += ts;
    constexpr float UpdateTime = 1.0f / 30.0f;
    if (m_CurrentTime >= UpdateTime)
    {
        m_CurrentTime -= UpdateTime;

        NumberOfCancerCell = 0;
        NumberOfHealthyCell = 0;
        NumberOfMedecineCell = 0;

        /*
        for (size_t i = 0; i < s_NumberOfThreads; i++)
        {
            updateCellsInPartition(s_NumberOfCellPerPartition * i);
        }
        */
        std::thread threads[s_NumberOfThreads];
        for (size_t i = 0; i < s_NumberOfThreads; i++)
        {
            threads[i] = std::thread(&CellArea::updateCellsInPartition, this, s_NumberOfCellPerPartition * i);
        }
        for (size_t i = 0; i < s_NumberOfThreads; i++)
        {
            threads[i].join();
        }
    }
}

void CellArea::updateCellsInPartition(size_t min)
{
    CellType partition[s_NumberOfCellPerPartition];
    std::copy_n(Types.begin() + min, s_NumberOfCellPerPartition, partition);
    for (size_t i = 0; i < s_NumberOfCellPerPartition; i++)
    {
        updateCell(partition, i, i + min);
    }

    for (size_t i = 0; i < s_NumberOfCellPerPartition; i++)
    {
        switch (Types[i + min])
        {
        case CellType::CANCER:
            NumberOfCancerCell++;
            break;
        case CellType::HEALTHY:
            NumberOfHealthyCell++;
            break;
        case CellType::MEDECINE:
            NumberOfMedecineCell++;
            break;
        }
    }
}

void CellArea::updateCell(CellType* partition, size_t partitionIndex, size_t index)
{
    switch (partition[partitionIndex])
    {
    case CellType::CANCER:
        break;
    case CellType::HEALTHY:
        {
            uint8_t count = 0;
            for (int32_t i : Neighbors[index])
            {
                if (partition[i] == CellType::CANCER)
                    count++;
            }
            
            if (count >= 6)
            {
                Colors[index] = { 1.0f, 0.0f, 0.0f, 1.0f };
                Types[index] = CellType::CANCER;
            }
        }   
        break;
    case CellType::MEDECINE:
        break;
    }
}

void CellArea::setNeighbor(size_t index, int32_t row, int32_t column, int32_t bound_x, int32_t bound_y)
{
    int32_t x = row - 1;
    int32_t y = column - 1;
    if (x >= 0 && x < bound_x && y >= 0 && y < bound_y)
        Neighbors[index].push_back(x + y * bound_x);
    x = row;
    y = column - 1;
    if (x >= 0 && x < bound_x && y >= 0 && y < bound_y)
        Neighbors[index].push_back(x + y * bound_x);
    x = row + 1;
    y = column - 1;
    if (x >= 0 && x < bound_x && y >= 0 && y < bound_y)
        Neighbors[index].push_back(x + y * bound_x);
    x = row - 1;
    y = column;
    if (x >= 0 && x < bound_x && y >= 0 && y < bound_y)
        Neighbors[index].push_back(x + y * bound_x);
    x = row + 1;
    y = column;
    if (x >= 0 && x < bound_x && y >= 0 && y < bound_y)
        Neighbors[index].push_back(x + y * bound_x);
    x = row - 1;
    y = column + 1;
    if (x >= 0 && x < bound_x && y >= 0 && y < bound_y)
        Neighbors[index].push_back(x + y * bound_x);
    x = row;
    y = column + 1;
    if (x >= 0 && x < bound_x && y >= 0 && y < bound_y)
        Neighbors[index].push_back(x + y * bound_x);
    x = row + 1;
    y = column + 1;
    if (x >= 0 && x < bound_x && y >= 0 && y < bound_y)
        Neighbors[index].push_back(x + y * bound_x);
}