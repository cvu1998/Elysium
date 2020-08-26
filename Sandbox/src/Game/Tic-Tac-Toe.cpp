#include "Tic-Tac-Toe.h"

TTTGrid::TTTGrid()
{
}

TTTGrid::~TTTGrid()
{
}

bool TTTGrid::isWinningMove(size_t index, uint32_t value) const
{
    return isWinningRow(index, value) || isWinningColumn(index, value) ||
        isWinningRightDiagonal(index, value) || isWinningLeftDiagonal(index, value);
}

void TTTGrid::getCurrentStateCode(std::string& string)
{
    std::stringstream ss;
    for (int i = 0; i < 3; i++)
        ss << getValue(i, 0) << getValue(i, 1) << getValue(i, 2);
    string = ss.str();
}

void TTTGrid::printGrid()
{
    ELY_INFO("----------");
    for (int i = 2; i >= 0; i--)
        ELY_INFO("{0} {1} {2}", getValue(i, 0), getValue(i, 1), getValue(i, 2));
    ELY_INFO("----------");
}

bool TTTGrid::isWinningRow(size_t index, uint32_t value) const
{
    uint16_t counter = 1;
    int32_t row = (int32_t)index / 3;
    int32_t column = (int32_t)index % 3;
    if (getValue(row, column + 1) == value)
    {
        counter++;
        if (getValue(row, column + 2) == value)
            counter++;
    }
    if (getValue(row, column - 1) == value)
    {
        counter++;
        if (getValue(row, column - 2) == value)
            counter++;
    }
    return counter >= 3;
}

bool TTTGrid::isWinningColumn(size_t index, uint32_t value) const
{
    uint16_t counter = 1;
    int32_t row = (int32_t)index / 3;
    int32_t column = (int32_t)index % 3;
    if (getValue(row + 1, column) == value)
    {
        counter++;
        if (getValue(row + 2, column) == value)
            counter++;
    }
    if (getValue(row - 1, column) == value)
    {
        counter++;
        if (getValue(row - 2, column) == value)
            counter++;
    }
    return counter >= 3;
}

bool TTTGrid::isWinningRightDiagonal(size_t index, uint32_t value) const
{
    uint16_t counter = 1;
    int32_t row = (int32_t)index / 3;
    int32_t column = (int32_t)index % 3;
    if (getValue(row + 1, column + 1) == value)
    {
        counter++;
        if (getValue(row + 2, column + 2) == value)
            counter++;
    }
    if (getValue(row - 1, column - 1) == value)
    {
        counter++;
        if (getValue(row - 2, column - 2) == value)
            counter++;
    }
    return counter >= 3;
}

bool TTTGrid::isWinningLeftDiagonal(size_t index, uint32_t value) const
{
    uint16_t counter = 1;
    int32_t row = (int32_t)index / 3;
    int32_t column = (int32_t)index % 3;
    if (getValue(row + 1, column - 1) == value)
    {
        counter++;
        if (getValue(row + 2, column - 2) == value)
            counter++;
    }
    if (getValue(row - 1, column + 1) == value)
    {
        counter++;
        if (getValue(row - 2, column + 2) == value)
            counter++;
    }
    return counter >= 3;
}