#pragma once

#include "TicTacToeGrid.h"

class TicTacToeMinimax
{
public:
    struct State
    {
        float Value = 0.0f;
        size_t Action = 0;
        size_t Depth = 0;
        bool Terminal = false;
        int Result = 0;

        TicTacToeGrid Grid;
        std::vector<TicTacToeMinimax::State> Children;

        State() = default;

        State(TicTacToeGrid grid) : Grid(grid)
        {
        }

        State(float value, size_t action, size_t depth, bool terminal, int result, TicTacToeGrid grid) : Value(value), Action(action), Depth(depth), Terminal(terminal), Result(result), Grid(grid)
        {
        }

        bool operator<(const TicTacToeMinimax::State& s) { return this->Value < s.Value; }
    };

private:
    TicTacToeGrid* m_Grid;

    static constexpr size_t s_GridSize = 9;

private:
    void generateTree(State& origin, int turn);

    int getValueFromExponent(int exponent);
    float evaluateState(const TicTacToeGrid& grid);
    int evaluateDiagonals(const TicTacToeGrid& grid);
    void generateChildren(std::vector<State>& states, const TicTacToeGrid& grid, int turn, size_t depth);

public:
    using GenerateChildren = std::function<void(std::vector<State>&, const TicTacToeGrid&, int, size_t)>;

    int Max = 0;     // Max
    int Min = 0;    // Min

    static constexpr size_t s_Depth = 2;

    GenerateChildren Generator = std::bind
    (
        &TicTacToeMinimax::generateChildren, 
        this, 
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3, 
        std::placeholders::_4
    );

public:
    TicTacToeMinimax(TicTacToeGrid* grid);

    size_t getAction(int turn);
    State getNStepState(int turn, float beta);
    State getGreedyNStepState(int turn);
};

