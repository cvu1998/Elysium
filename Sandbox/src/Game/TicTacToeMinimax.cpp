#include "TicTacToeMinimax.h"

TicTacToeMinimax::TicTacToeMinimax(TicTacToeGrid* grid) : m_Grid(grid)
{
}

size_t TicTacToeMinimax::playAction()
{
    TicTacToeState origin = { 0, *m_Grid };
    generateChildren(origin.Children, origin, Minimax, false);

    for (TicTacToeState& s : origin.Children)
    {
        generateChildren(s.Children, s, Opponent, true);
    }

    for (TicTacToeState& s : origin.Children)
    {
        if (!s.Children.empty())
        {
            TicTacToeState minState = *std::min_element(s.Children.begin(), s.Children.end());
            s.Value = minState.Value;
        }
    }

    size_t actionIndex = 0;
    size_t index = 0;
    origin.Value = origin.Children[0].Value;
    for (size_t i = 1; i < origin.Children.size(); i++)
    {
        if (origin.Children[i].Value > origin.Value)
        {
            origin.Value = origin.Children[i].Value;
            index = i;
        }
    }

    for (size_t i = 0; i < 9; i++)
    {
        if (origin.StateRep.Grid[i] != origin.Children[index].StateRep.Grid[i])
        {
            actionIndex = i;
            break;
        }
    }

    return actionIndex;
}

int TicTacToeMinimax::getValueFromExponent(int exponent)
{
    int value = 0;
    if (exponent > 0)
    {
        int gain = 1;
        for (int z = 1; z < exponent; z++)
            gain += 10 * z;

        value += gain;
    }
    else if (exponent < 0)
    {
        int loss = -1;
        for (int z = -1; z > exponent; z--)
            loss += 10 * z;

        value += loss;
    }
    return value;
}

int TicTacToeMinimax::evaluateState(const TicTacToeGrid& grid)
{
    int value = 0;
    for (int32_t i = 0; i < 3; i++)
    {
        int exponent = 0;
        for (int32_t j = 0; j < 3; j++)
        {
            if (grid.getValue(i, j) == Minimax)
                exponent++;
            else if (grid.getValue(i, j) == Opponent)
                exponent--;
        }
        value += getValueFromExponent(exponent);
    }

    for (int32_t i = 0; i < 3; i++)
    {
        int exponent = 0;
        for (int32_t j = 0; j < 3; j++)
        {
            if (grid.getValue(j, i) == Minimax)
                exponent++;
            else if (grid.getValue(j, i) == Opponent)
                exponent--;
        }
        value += getValueFromExponent(exponent);
    }
    value += evaluateDiagonals(grid);

    return value;
}

int TicTacToeMinimax::evaluateDiagonals(const TicTacToeGrid& grid)
{
    int value = 0;
    int exponent = 0;
    for (int32_t i = 0; i < 3; i++)
    {
        if (grid.getValue(i, i) == Minimax)
            exponent++;
        else if (grid.getValue(i, i) == Opponent)
            exponent--;
    }
    value += getValueFromExponent(exponent);

    exponent = 0;
    for (int32_t i = 0; i < 3; i++)
    {
        if (grid.getValue(2 - i, i) == Minimax)
            exponent++;
        else if (grid.getValue(2 - i, i) == Opponent)
            exponent--;
    }
    value += getValueFromExponent(exponent);

    return value;
}

void TicTacToeMinimax::generateChildren(std::vector<TicTacToeState>& states, const TicTacToeState& state, int32_t turn, bool lastLayer)
{
    for (size_t i = 0; i < 9; i++)
    {
        TicTacToeGrid newGrid = state.StateRep;
        if (newGrid.isValid(i))
        {
            newGrid.Grid[i] = turn;
            if (lastLayer)
            {
                states.emplace_back(ValueFunction(newGrid), newGrid);
                continue;
            }
            states.emplace_back(0, newGrid);
        }
    }
}