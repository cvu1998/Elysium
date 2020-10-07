#include "CellArea.h"

CellArea::CellArea(Elysium::Vector2 offset)
{
    float percentage = Random::Float();
    constexpr size_t MinimumNumberOfCancerCell = NumberOfCells / 4;
    NumberOfCancerCells = (unsigned int)(percentage * MinimumNumberOfCancerCell) + MinimumNumberOfCancerCell;
    unsigned int counter = 0;
    std::unordered_set<size_t> indexes;
    while (counter < NumberOfCancerCells)
    {
        size_t index = (size_t)Random::Integer(0, NumberOfCells);
        while (indexes.find(index) != indexes.end())
            index = (size_t)Random::Integer(0, NumberOfCells);
        indexes.insert(index);
        counter++;
    }

    for (size_t i = 0; i < NumberOfCells; i++)
    {
        if (i < s_NumberOfCellsPerPartition)
            setNeighbor((int)i);

        Positions[i] = { ((float)(i % NumberOfCell_X) - offset.x) * m_CellSize, ((float)(i / NumberOfCell_X) - offset.y) * m_CellSize };
        if (indexes.find(i) != indexes.end())
        {
            Colors[i] = s_ColorRed;
            m_Types[i] = CellType::CANCER;
        }
        else
        {
            Colors[i] = s_ColorGreen;
            m_Types[i] = CellType::HEALTHY;
            NumberOfHealthyCells++;
        }
    }

    Elysium::Renderer2D::setPointSize(m_CellSize);

    ELY_INFO("Number of cell per partition: {0}", s_NumberOfCellsPerPartition);
    ELY_INFO("Number of threads: {0}", std::thread::hardware_concurrency());
}

void CellArea::onUpdate(Elysium::Timestep ts)
{
    m_CurrentTime += ts;
    constexpr float UpdateTime = 1.0f / 30.0f;
    if (m_CurrentTime >= UpdateTime)
    {
        m_CurrentTime -= UpdateTime;

        NumberOfCancerCells = 0;
        NumberOfHealthyCells = 0;
        NumberOfMedecineCells = 0;

        if (!m_InputBuffer.empty())
        {
            for (size_t i : m_InputBuffer)
            {
                int counter = 0;
                int numberOfCells = Random::Integer(1, 8);
                for (int j : m_Neighbors[i % s_NumberOfCellsPerPartition])
                {
                    counter++;
                    if (counter >= numberOfCells)
                        break;

                    size_t index = j + ((i + 1) / s_NumberOfCellsPerPartition) * s_NumberOfCellsPerPartition;
                    if (m_MedecineCells.find(index) == m_MedecineCells.end())
                    {
                        m_MedecineCells.insert({ index, { m_Types[index], (int)index - (int)i } });
                        m_Types[index] = CellType::MEDECINE;
                    }
                }
            }
            m_InputBuffer.clear();
        }

        std::thread threads[s_NumberOfThreads];
        CellType* partitions[s_NumberOfThreads];
        PartitionStats stats[s_NumberOfThreads];
        std::unordered_map<size_t, MedecineCell> medecineMap[s_NumberOfThreads];
        for (size_t i = 0; i < s_NumberOfThreads; i++)
        {
            partitions[i] = new CellType[s_NumberOfCellsPerPartition];
            std::copy_n(m_Types.begin() + s_NumberOfCellsPerPartition * i, s_NumberOfCellsPerPartition, partitions[i]);
            threads[i] = std::thread(&CellArea::updateCellsInPartition, this, partitions[i], std::ref(stats[i]), std::ref(medecineMap[i]), s_NumberOfCellsPerPartition * i);
        }
        for (size_t i = 0; i < s_NumberOfThreads; i++)
        {
            threads[i].join();
        }
        m_MedecineCells.clear();

        for (size_t i = 0; i < s_NumberOfThreads; i++)
        {
            m_MedecineCells.insert(medecineMap[i].begin(), medecineMap[i].end());
            NumberOfCancerCells += stats[i].NumberOfCancerCells;
            NumberOfHealthyCells += stats[i].NumberOfHealthyCells;
            NumberOfMedecineCells += stats[i].NumberOfMedecineCells;
            delete[] partitions[i];
        }
    }
}

void CellArea::updateCellsInPartition(CellType* partition, PartitionStats& stats, 
    std::unordered_map<size_t, MedecineCell>& medecineMap,
    size_t min)
{
    std::unordered_set<size_t> updatedMedecine;
    for (size_t i = 0; i < s_NumberOfCellsPerPartition; i++)
    {
        switch (partition[i])
        {
        case CellType::CANCER:
        {
            size_t counter = 0;
            size_t medecineCells[8];
            for (int j : m_Neighbors[i])
            {
                if (partition[j] == CellType::MEDECINE)
                    medecineCells[counter++] = (size_t)j + min;
            }

            if (counter >= 6)
            {
                m_Types[i + min] = CellType::HEALTHY;
                for (size_t j = 0; j < counter; j++)
                {
                    m_Types[medecineCells[j]] = CellType::HEALTHY;
                    updatedMedecine.insert(medecineCells[j]);
                }
            }
        }
            break;
        case CellType::HEALTHY:
        {
            uint8_t cancerCount = 0;
            for (int j : m_Neighbors[i])
                cancerCount += partition[j] == CellType::CANCER ? 1 : 0;

            if (cancerCount >= 6)
                m_Types[i + min] = CellType::CANCER;
        }
            break;
        }
    }

    for (size_t i = 0; i < s_NumberOfCellsPerPartition; i++)
    {
        switch (m_Types[i + min])
        {
        case CellType::CANCER:
            Colors[i + min] = s_ColorRed;
            stats.NumberOfCancerCells++;
            break;
        case CellType::HEALTHY:
            Colors[i + min] = s_ColorGreen;
            stats.NumberOfHealthyCells++;
            break;
        case CellType::MEDECINE:
            moveMedecineCells(i + min, medecineMap, updatedMedecine);
            Colors[i + min] = s_ColorYellow;
            stats.NumberOfMedecineCells++;
            break;
        }
    }
}

void CellArea::moveMedecineCells(size_t cellIndex,
    std::unordered_map<size_t, MedecineCell>& medecineMap,
    std::unordered_set<size_t>& updatedMedecine)
{
    if (updatedMedecine.find(cellIndex) == updatedMedecine.end())
    {
        auto iterators = m_MedecineCells.equal_range(cellIndex);
        for (auto it = iterators.first; it != iterators.second; it++)
        {
            if (it->second.PreviousType != CellType::MEDECINE)
                m_Types[cellIndex] = it->second.PreviousType;

            int index = (int)cellIndex + it->second.offset;
            int xOffset = (it->second.offset < 0) ? it->second.offset + (int)NumberOfCell_X : it->second.offset - (int)NumberOfCell_X;
            xOffset = abs(it->second.offset) == 1 ? it->second.offset : xOffset;
            int yOffset = (it->second.offset < 0) ? (it->second.offset - 1) / (int)NumberOfCell_Y : (it->second.offset + 1) / (int)NumberOfCell_Y;
            int x = (int)cellIndex % NumberOfCell_X;
            int y = (int)cellIndex / NumberOfCell_X;
            if (x + xOffset >= 0 && (size_t)(x + xOffset) < NumberOfCell_X && y + yOffset >= 0 && (size_t)(y + yOffset) < NumberOfCell_Y &&
                index >= 0 && index < NumberOfCells)
            {
                updatedMedecine.insert({ cellIndex });
                medecineMap.insert({ (size_t)index, { m_Types[index], it->second.offset } });
                m_Types[index] = CellType::MEDECINE;
            }
        }
    }
}

void CellArea::setNeighbor(int index)
{
    for (size_t i = 0; i < 8; i++)
    {
        int x1 = index % (int)NumberOfCell_X;
        int y1 = index / (int)NumberOfCell_X;
        int partitionIndex = s_NeighborIndexes[i] + index;
        int x2 = partitionIndex % (int)NumberOfCell_X;
        int y2 = partitionIndex / (int)NumberOfCell_X;
        int xOffset = (s_NeighborIndexes[i] < 0) ? s_NeighborIndexes[i] + (int)NumberOfCell_X : s_NeighborIndexes[i] - (int)NumberOfCell_X;
        xOffset = abs(s_NeighborIndexes[i]) == 1 ? s_NeighborIndexes[i] : xOffset;
        int yOffset = (s_NeighborIndexes[i] < 0) ? (s_NeighborIndexes[i] - 1) / (int)NumberOfCell_Y : (s_NeighborIndexes[i] + 1) / (int)NumberOfCell_Y;
        if (x1 + xOffset == x2 && y1 + yOffset == y2 && partitionIndex >= 0 && partitionIndex < s_NumberOfCellsPerPartition)
            m_Neighbors[index].push_back(partitionIndex);
    }
}

size_t CellArea::getIndex(const Elysium::Vector2& position)
{
    size_t x = 0;
    for (size_t i = 0; i < NumberOfCell_X; i++)
    {
        if (position.x <= Positions[i].x && i == 0)
        {
            x = 0;
            break;
        }
        else if (position.x <= Positions[i].x)
        {
            x = i - 1;
            break;
        }
        else if (position.x > Positions[i].x && i == NumberOfCell_X - 1)
        {
            x = NumberOfCell_X - 1;
            break;
        }
    }
    int y = -1;
    for (size_t i = 0; i < NumberOfCells; i += NumberOfCell_X)
    {
        y++;
        if (position.y <= Positions[i].y)
            break;
        else if (position.y > Positions[i].y && i == NumberOfCell_Y - 1)
            break;
    }
    return x + (size_t)y * NumberOfCell_X;
}

void CellArea::injectMedecine(const Elysium::Vector2& position)
{
    m_InputBuffer.insert(getIndex(position));
}