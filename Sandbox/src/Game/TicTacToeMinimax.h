#pragma once

#include "TicTacToeGrid.h"

class TicTacToeMinimax
{
private:
    struct TicTacToeState
    {
        int Value = 0;

        TicTacToeGrid StateRep;
        std::vector<TicTacToeState> Children;

        TicTacToeState(int value, TicTacToeGrid grid) :
            Value(value),
            StateRep(grid)
        {
        }

        bool operator<(const TicTacToeState& s)
        {
            return this->Value < s.Value;
        }
    };


    TicTacToeGrid* m_Grid;

private:
    int getValueFromExponent(int exponent);
    int evaluateState(const TicTacToeGrid& grid);
    int evaluateDiagonals(const TicTacToeGrid& grid);
    void generateChildren(std::vector<TicTacToeState>& states, const TicTacToeState& state, uint32_t turn, bool lastLayer);

public:
    uint32_t Minimax = 0;     // Max
    uint32_t Opponent = 0;    // Min

public:
    TicTacToeMinimax(TicTacToeGrid* grid);

    size_t playAction();
};

