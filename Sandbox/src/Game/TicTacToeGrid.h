#pragma once

#include <Elysium.h>

class TicTacToeGrid
{
public:
    std::array<int, 9> Grid = { 0 };

private:
    bool isWinningRow(size_t index, int value) const;
    bool isWinningColumn(size_t index, int value) const;
    bool isWinningRightDiagonal(size_t index, int value) const;
    bool isWinningLeftDiagonal(size_t index, int value) const;

public:
    TicTacToeGrid();
    ~TicTacToeGrid();

    inline bool isValid(size_t index) const { return Grid[index] == 0; }
    inline int getValue(int row, int column) const
    {
        if (row >= 0 && row < 3 && column >= 0 && column < 3)
            return Grid[(size_t)row * 3 + (size_t)column];
        return 0;
    }

    inline void clear()
    {
        Grid.fill(0);
    }

    bool isWinningMove(size_t index, int value) const;
    bool isFilled() const;
    void printGrid() const;
};