#include "TicTacToeGrid.h"

TicTacToeGrid::TicTacToeGrid()
{
}

TicTacToeGrid::~TicTacToeGrid()
{
}

bool TicTacToeGrid::isWinningMove(size_t index, int value) const
{
    return isWinningRow(index, value) || isWinningColumn(index, value) ||
        isWinningRightDiagonal(index, value) || isWinningLeftDiagonal(index, value);
}

bool TicTacToeGrid::isEmpty() const
{
    for (int i : Grid)
        if (i != 0) return false;
    return true;
}

bool TicTacToeGrid::isFilled() const
{
    for (int i : Grid)
        if (i == 0) return false;
    return true;
}

void TicTacToeGrid::printGrid() const
{
    ELY_INFO("----------");
    for (int i = 2; i >= 0; i--)
        ELY_INFO("{0} {1} {2}", getValue(i, 0), getValue(i, 1), getValue(i, 2));
    ELY_INFO("----------");
}

bool TicTacToeGrid::isWinningRow(size_t index, int value) const
{
    uint16_t counter = 1;
    int row = (int)index / 3;
    int column = (int)index % 3;
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

bool TicTacToeGrid::isWinningColumn(size_t index, int value) const
{
    uint16_t counter = 1;
    int row = (int)index / 3;
    int column = (int)index % 3;
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

bool TicTacToeGrid::isWinningRightDiagonal(size_t index, int value) const
{
    uint16_t counter = 1;
    int row = (int)index / 3;
    int column = (int)index % 3;
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

bool TicTacToeGrid::isWinningLeftDiagonal(size_t index, int value) const
{
    uint16_t counter = 1;
    int row = (int)index / 3;
    int column = (int)index % 3;
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