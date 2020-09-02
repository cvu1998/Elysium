#pragma once

#include <Elysium.h>

class TTTGrid
{
public:
    std::array<uint32_t, 9> Grid = { 0 };

private:
    bool isWinningRow(size_t index, uint32_t value) const;
    bool isWinningColumn(size_t index, uint32_t value) const;
    bool isWinningRightDiagonal(size_t index, uint32_t value) const;
    bool isWinningLeftDiagonal(size_t index, uint32_t value) const;

public:
    TTTGrid();
    ~TTTGrid();

    inline bool isValid(size_t index) const { return Grid[index] == 0; }
    inline uint32_t getValue(int32_t row, int32_t column) const
    {
        if (row >= 0 && row < 3 && column >= 0 && column < 3)
            return Grid[(size_t)row * 3 + (size_t)column];
        return 0;
    }

    inline void clear()
    {
        Grid.fill(0);
    }

    bool isWinningMove(size_t index, uint32_t value) const;
    void getCurrentStateCode(std::string& string);
    void printGrid();
};

struct State
{
    int Value = 0;

    TTTGrid StateRep;
    std::vector<State> Children;

    State(int value, TTTGrid grid) :
        Value(value),
        StateRep(grid)
    {
    }

    bool operator<(const State& s)
    {
        return this->Value < s.Value;
    }
};

class MinimaxPlayer
{
private:
    TTTGrid* m_Grid;

private:
    int evaluateState(const TTTGrid& grid);
    int evaluateDiagonals(const TTTGrid& grid);
    void generateChildren(std::vector<State>& states, const State& state, uint32_t turn, bool lastLayer);

public:
    uint32_t Minimax = 0;     // Max
    uint32_t Opponent = 0;    // Min

public:
    MinimaxPlayer(TTTGrid* grid);

    size_t playAction();

};