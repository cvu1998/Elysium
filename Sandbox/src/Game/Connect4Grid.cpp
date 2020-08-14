#include "Connect4Grid.h"

Connect4Grid::Connect4Grid()
{
}

Connect4Grid::~Connect4Grid()
{
}

bool Connect4Grid::isWinningMove(size_t index, uint32_t value) const
{
    return isWinningRow(index, value) || isWinningColumn(index, value) || 
        isWinningRightDiagonal(index, value) || isWinningLeftDiagonal(index, value);
}

void Connect4Grid::printGrid()
{
    ELY_INFO("----------");
    for (int i = 6; i >= 0; i--)
        ELY_INFO("{0} {1} {2} {3} {4} {5} {6}", getValue(i, 0), getValue(i, 1), getValue(i, 2), getValue(i, 3), \
            getValue(i, 4), getValue(i, 5), getValue(i, 6));
    ELY_INFO("----------");
}

bool Connect4Grid::isWinningRow(size_t index, uint32_t value) const
{
    uint16_t counter = 1;
    int32_t row = (int32_t)m_ColumnIndexes[index] - 1;
    int32_t column = (int32_t)index;
    if (getValue(row, column + 1) == value)
    {
        counter++;
        if (getValue(row, column + 2) == value)
        {
            counter++;
            if (getValue(row, column + 3) == value)
                counter++;
        }
    }
    if (getValue(row, column - 1) == value)
    {
        counter++;
        if (getValue(row, column - 2) == value)
        {
            counter++;
            if (getValue(row, column - 3) == value)
                counter++;
        }
    }
    return counter == 4;
}

bool Connect4Grid::isWinningColumn(size_t index, uint32_t value) const
{
    uint16_t counter = 1;
    int32_t row = (int32_t)m_ColumnIndexes[index] - 1;
    int32_t column = (int32_t)index;
    if (getValue(row + 1, column) == value)
    {
        counter++;
        if (getValue(row + 2, column) == value)
        {
            counter++;
            if (getValue(row + 3, column) == value)
                counter++;
        }
    }
    if (getValue(row - 1, column) == value)
    {
        counter++;
        if (getValue(row - 2, column) == value)
        {
            counter++;
            if (getValue(row - 3, column) == value)
                counter++;
        }
    }
    return counter == 4;
}

bool Connect4Grid::isWinningRightDiagonal(size_t index, uint32_t value) const
{
    uint16_t counter = 1;
    int32_t row = (int32_t)m_ColumnIndexes[index] - 1;
    int32_t column = (int32_t)index;
    if (getValue(row + 1, column + 1) == value)
    {
        counter++;
        if (getValue(row + 2, column + 2) == value)
        {
            counter++;
            if (getValue(row + 3, column + 3) == value)
                counter++;
        }
    }
    if (getValue(row - 1, column - 1) == value)
    {
        counter++;
        if (getValue(row - 2, column - 2) == value)
        {
            counter++;
            if (getValue(row - 3, column - 3) == value)
                counter++;
        }
    }
    return counter == 4;
}

bool Connect4Grid::isWinningLeftDiagonal(size_t index, uint32_t value) const
{
    uint16_t counter = 1;
    int32_t row = (int32_t)m_ColumnIndexes[index] - 1;
    int32_t column = (int32_t)index;
    if (getValue(row + 1, column - 1) == value)
    {
        counter++;
        if (getValue(row + 2, column - 2) == value)
        {
            counter++;
            if (getValue(row + 3, column - 3) == value)
                counter++;
        }
    }
    if (getValue(row - 1, column + 1) == value)
    {
        counter++;
        if (getValue(row - 2, column + 2) == value)
        {
            counter++;
            if (getValue(row - 3, column + 3) == value)
                counter++;
        }
    }
    return counter == 4;
}