#include "TicTacToeMinimax.h"

TicTacToeMinimax::TicTacToeMinimax(TicTacToeGrid* grid) : m_Grid(grid)
{
}

void TicTacToeMinimax::generateTree(State& origin, int turn)
{
    int first = Max == turn ? Max : Min;
    int second = first == Max ? Min : Max;
    Generator(origin.Children, origin.Grid, first, 1);

    for (State& s : origin.Children)
    {
        if (!s.Terminal)
        {
            ELY_INFO("Minimax Action: {0}", s.Action);
            Generator(s.Children, s.Grid, second, 2);
        }
    }

    for (State& s : origin.Children)
    {
        if (!s.Children.empty())
        {
            State state;
            if (first == Max) state = *std::min_element(s.Children.begin(), s.Children.end());
            else state = *std::max_element(s.Children.begin(), s.Children.end());

            s.Value = state.Value;
            s.Depth = state.Depth;
            s.Terminal = state.Terminal;
            s.Result = state.Result;
            s.TargetGrid = state.Grid;
        }
    }
}

void TicTacToeMinimax::generateTreestrap(State& origin, int turn, std::vector<State>& states)
{
    int first = Max == turn ? Max : Min;
    int second = first == Max ? Min : Max;
    Generator(origin.Children, origin.Grid, first, 1);

    for (State& s : origin.Children)
    {
        if (!s.Terminal)
        {
            ELY_INFO("Minimax Action: {0}", s.Action);
            Generator(s.Children, s.Grid, second, 2);
        }
    }

    for (State& s : origin.Children)
    {
        if (!s.Children.empty())
        {
            for (State& c : s.Children)
                if (c.Terminal) states.push_back(s);

            State state;
            if (first == Max) state = *std::min_element(s.Children.begin(), s.Children.end());
            else state = *std::max_element(s.Children.begin(), s.Children.end());

            s.Value = state.Value;
            s.Terminal = state.Terminal;
            s.Result = state.Result;
            s.TargetGrid = state.Grid;
        }
    }
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

float TicTacToeMinimax::evaluateState(const TicTacToeGrid& grid)
{
    int value = 0;
    for (int32_t i = 0; i < 3; i++)
    {
        int exponent = 0;
        for (int32_t j = 0; j < 3; j++)
        {
            if (grid.getValue(i, j) == Max)
                exponent++;
            else if (grid.getValue(i, j) == Min)
                exponent--;
        }
        value += getValueFromExponent(exponent);
    }

    for (int i = 0; i < 3; i++)
    {
        int exponent = 0;
        for (int j = 0; j < 3; j++)
        {
            if (grid.getValue(j, i) == Max)
                exponent++;
            else if (grid.getValue(j, i) == Min)
                exponent--;
        }
        value += getValueFromExponent(exponent);
    }
    value += evaluateDiagonals(grid);

    return (float)value;
}

int TicTacToeMinimax::evaluateDiagonals(const TicTacToeGrid& grid)
{
    int value = 0;
    int exponent = 0;
    for (int i = 0; i < 3; i++)
    {
        if (grid.getValue(i, i) == Max)
            exponent++;
        else if (grid.getValue(i, i) == Min)
            exponent--;
    }
    value += getValueFromExponent(exponent);

    exponent = 0;
    for (int i = 0; i < 3; i++)
    {
        if (grid.getValue(2 - i, i) == Max)
            exponent++;
        else if (grid.getValue(2 - i, i) == Min)
            exponent--;
    }
    value += getValueFromExponent(exponent);

    return value;
}

void TicTacToeMinimax::generateChildren(std::vector<State>& states, const TicTacToeGrid& grid, int turn, size_t depth)
{
    for (size_t i = 0; i < s_GridSize; i++)
    {
        TicTacToeGrid next = grid;
        if (next.isValid(i))
        {
            next.Grid[i] = turn;
            if (grid.isWinningMove(i, turn) || next.isFilled())
            {
                int result = grid.isWinningMove(i, turn) ? turn : 0;
                states.emplace_back(evaluateState(next), i, depth, true, result, next);
                continue;
            }
            else if (depth == TicTacToeMinimax::s_Depth)
            {
                states.emplace_back(evaluateState(next), i, depth, false, 0, next);
                continue;
            }
            states.emplace_back(0.0f, i, depth, false, 0, next);
        }
    }
}

size_t TicTacToeMinimax::getAction(int turn)
{
    State origin(*m_Grid);
    generateTree(origin, turn);

    State s = origin.Children[0];
    printf("V(%zd) : %f ", origin.Children[0].Action, origin.Children[0].Value);
    for (size_t i = 1; i < origin.Children.size(); i++)
    {
        if (Max == turn && origin.Children[i].Value > s.Value) s = origin.Children[i];
        else if (Min == turn && origin.Children[i].Value < s.Value) s = origin.Children[i];
        printf("V(%zd) : %f ", origin.Children[i].Action, origin.Children[i].Value);
    }
    printf("\n");

    return s.Action;
}

TicTacToeMinimax::State TicTacToeMinimax::getNStepState(int turn, float beta)
{
    State origin(*m_Grid);
    generateTree(origin, turn);

    std::vector<float> stateValues(origin.Children.size());
    for (size_t i = 0; i < origin.Children.size(); i++)
    {
        stateValues[i] = origin.Children[i].Value;
        printf("V(%zd) : %f ", origin.Children[i].Action, stateValues[i]);
    }
    printf("\n");

    std::vector<float> distribution;
    if (Max == turn) Elysium::AI::softmax(stateValues, distribution, beta);
    else Elysium::AI::softmin(stateValues, distribution, beta);

    float random = Elysium::Random::Float();

    bool found = false;

    float probability = 0.0f;
    State s;
    for (size_t i = 0; i < distribution.size(); ++i)
    {
        probability += distribution[i];
        if (random < probability && !found)
        {
            s = origin.Children[i];
            found = true;
        }

        printf("P(%zd) : %f ", origin.Children[i].Action, distribution[i]);
    }
    printf("\n");

    ELY_INFO("Random: {0}", random);

    return s;
}

TicTacToeMinimax::State TicTacToeMinimax::getGreedyNStepState(int turn)
{
    State origin(*m_Grid);
    generateTree(origin, turn);

    State s = origin.Children[0];
    printf("V(%zd) : %f ", origin.Children[0].Action, origin.Children[0].Value);
    for (size_t i = 1; i < origin.Children.size(); i++)
    {
        if (Max == turn && origin.Children[i].Value > s.Value) s = origin.Children[i];
        else if (Min == turn && origin.Children[i].Value < s.Value) s = origin.Children[i];
        printf("V(%zd) : %f ", origin.Children[i].Action, origin.Children[i].Value);
    }
    printf("\n");

    return s;
}

TicTacToeMinimax::TreestrapData TicTacToeMinimax::treestrap(int turn)
{
    std::vector<TicTacToeMinimax::State> states;

    State origin(*m_Grid);
    generateTreestrap(origin, turn, states);

    State s = origin.Children[0];
    std::vector<float> stateValues(origin.Children.size());
    printf("V(%zd) : %f ", origin.Children[0].Action, origin.Children[0].Value);
    for (size_t i = 1; i < origin.Children.size(); i++)
    {
        stateValues[i] = origin.Children[i].Value;
        printf("V(%zd) : %f ", origin.Children[i].Action, origin.Children[i].Value);
        if (Max == turn && origin.Children[i].Value > s.Value) s = origin.Children[i];
        else if (Min == turn && origin.Children[i].Value < s.Value) s = origin.Children[i];
    }
    printf("\n");

    states.insert(states.end(), origin.Children.begin(), origin.Children.end());
    states.push_back(s);

    for (size_t i = 0; i < origin.Children.size(); i++)
        stateValues[i] = origin.Children[i].Value;

    std::vector<float> distribution;
    if (Max == turn) Elysium::AI::softmax(stateValues, distribution, 5.0f);
    else Elysium::AI::softmin(stateValues, distribution, 5.0f);

    float random = Elysium::Random::Float();

    bool found = false;

    float probability = 0.0f;
    size_t action = origin.Children[0].Action;
    for (size_t i = 0; i < distribution.size(); ++i)
    {
        probability += distribution[i];
        if (random < probability && !found)
        {
            action = origin.Children[i].Action;
            found = true;
        }

        printf("P(%zd) : %f ", origin.Children[i].Action, distribution[i]);
    }
    printf("\n");

    return { states, action };
}
