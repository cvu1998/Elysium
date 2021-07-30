#pragma once

#include "TicTacToeGrid.h"

class TicTacToeMinimax
{
private:
    struct TicTacToeState
    {
        float Value = 0;

        TicTacToeGrid StateRep;
        std::vector<TicTacToeState> Children;

        TicTacToeState(float value, TicTacToeGrid grid) :
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
    float evaluateState(const TicTacToeGrid& grid);
    int evaluateDiagonals(const TicTacToeGrid& grid);
    void generateChildren(std::vector<TicTacToeState>& states, const TicTacToeState& state, int turn, bool lastLayer);

public:
    using StateValueFunction = std::function<float(const TicTacToeGrid&)>;

    int Minimax = 0;     // Max
    int Opponent = 0;    // Min

    StateValueFunction ValueFunction = std::bind(&TicTacToeMinimax::evaluateState, this, std::placeholders::_1);

public:
    TicTacToeMinimax(TicTacToeGrid* grid);

    size_t playAction();
};

