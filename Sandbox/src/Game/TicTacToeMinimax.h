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
    using StateValueFunction = std::function<int(const TicTacToeGrid&)>;

    uint32_t Minimax = 0;     // Max
    uint32_t Opponent = 0;    // Min

    StateValueFunction ValueFunction = std::bind(&TicTacToeMinimax::evaluateState, this, std::placeholders::_1);

public:
    TicTacToeMinimax(TicTacToeGrid* grid);

    size_t playAction();
};

