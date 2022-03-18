#include "TicTacToeScene.h"

#include "Game/Systems.h"

TicTacToeScene::TicTacToeScene(unsigned int width, unsigned int height) :
    Elysium::Scene("Tic-Tac-Toe"),
    m_Camera(-m_Height * (float)(width / height), m_Height* (float)(width / height), -m_Height * 0.5f, m_Height * 0.5f),
    m_X("res/texture/TicTacToeX.png"),
    m_O("res/texture/TicTacToeO.png"),
    m_Minimax(&m_Grid),
    m_TicTacToeModel(s_InputSize),
    m_TargetModel(s_InputSize),
    m_ModelMinimax(&m_Grid),
    m_OpponentMinimax(&m_Grid),
    m_EpisodeData(0, s_DataLength, true)
{
    m_CoinTextures[0] = m_X.getTextureData();
    m_CoinTextures[1] = m_O.getTextureData();

    m_Camera.setPosition({ 0.0f, 0.0f, 0.0f });

    m_Minimax.Max = 1;
    m_Minimax.Min = -1;

    m_TicTacToeModel.add(new Elysium::Dense(s_HiddenSize, Elysium::AI::Activation::LEAKY_RELU, Elysium::AI::Initializer::HE));
    m_TicTacToeModel.add(new Elysium::Dense(s_HiddenSize, Elysium::AI::Activation::LEAKY_RELU, Elysium::AI::Initializer::HE));
    m_TicTacToeModel.add(new Elysium::Dense(s_OutputSize, Elysium::AI::Activation::LINEAR));
    m_TicTacToeModel.LossFunction = Elysium::AI::Loss::MEAN_SQUARED;
    m_TicTacToeModel.LearningRate = s_LearningRate;

    m_TargetModel.add(new Elysium::Dense(s_HiddenSize, Elysium::AI::Activation::LEAKY_RELU, Elysium::AI::Initializer::HE));
    m_TargetModel.add(new Elysium::Dense(s_HiddenSize, Elysium::AI::Activation::LEAKY_RELU, Elysium::AI::Initializer::HE));
    m_TargetModel.add(new Elysium::Dense(s_OutputSize, Elysium::AI::Activation::LINEAR));

    m_OpponentModels.reserve(4);
    for (size_t i = 0; i < 4; ++i)
    {
        m_OpponentModels.emplace_back(s_InputSize);
        m_OpponentModels[i].add(new Elysium::Dense(s_HiddenSize, Elysium::AI::Activation::LEAKY_RELU, Elysium::AI::Initializer::HE));
        m_OpponentModels[i].add(new Elysium::Dense(s_HiddenSize, Elysium::AI::Activation::LEAKY_RELU, Elysium::AI::Initializer::HE));
        m_OpponentModels[i].add(new Elysium::Dense(s_OutputSize, Elysium::AI::Activation::LINEAR));

        std::string path = "res/AI/Tic-Tac-Toe/opponent-model";
        std::string name = path + '-' + std::to_string(i);
        m_OpponentModels[i].load(name.c_str());
    }

    m_TicTacToeModel.load("res/AI/Tic-Tac-Toe/tictactoe-model");

    updateTarget();

    m_TicTacToeModel.summary();

    m_ModelMinimax.Generator = [this](std::vector<TicTacToeMinimax::State>& states, const TicTacToeGrid& grid, int turn, size_t depth)
    {
        modelGenerator(states, grid, turn, depth);
    };

    m_ModelMinimax.Max = 1;
    m_ModelMinimax.Min = -1;

    m_OpponentMinimax.Generator = [this](std::vector<TicTacToeMinimax::State>& states, const TicTacToeGrid& grid, int turn, size_t depth)
    {
        opponentGenerator(states, grid, turn, depth);
    };

    m_OpponentMinimax.Max = -1;
    m_OpponentMinimax.Min = 1;

    saveModels();
}

TicTacToeScene::~TicTacToeScene()
{
    saveModels();
}

Elysium::Vector2 TicTacToeScene::getPosition(Elysium::Action action)
{
    switch (action)
    {
    case 1:
        return { 0.0f, -6.0f };
        break;
    case 2:
        return { 6.0f, -6.0f };
        break;
    case 3:
        return { -6.0f, 0.0f };
        break;
    case 4:
        return { 0.0f, 0.0f };
        break;
    case 5:
        return { 6.0f, 0.0f };
        break;
    case 6:
        return { -6.0f, 6.0f };
        break;
    case 7:
        return { 0.0f, 6.0f };
        break;
    case 8:
        return { 6.0f, 6.0f };
        break;
    default:
        return { -6.0f, -6.0f };
        break;
    }
}

void TicTacToeScene::addAction(Elysium::Vector2 position, size_t index)
{
    if (m_Grid.isValid(index))
    {
        m_MoveCooldown = s_Cooldown;

        m_Coins[m_CoinIndex] = position;
        m_CoinsTextureIndexes[m_CoinIndex++] = m_Turn == -1 ? 1 : 2;
        m_Grid.Grid[index] = m_Turn;

        if (m_CoinIndex >= 9)
        {
            m_GameOver = true;
            m_Tie = true;
        }

        if (m_Grid.isWinningMove(index, m_Turn))
        {
            m_GameOver = true;
            m_Tie = false;
            switch (m_Turn)
            {
            case -1:
                m_XScore++;
                break;
            case 1:
                m_OScore++;
                break;
            }
        }

        switch (m_Turn)
        {
        case -1:
            m_Turn = 1;
            break;
        case 1:
            m_Turn = -1;
            break;
        }
    }
}

bool TicTacToeScene::isWithinBounds(Elysium::Vector2 position, float x1, float y1, float x2, float y2)
{
    bool CollisionX = position.x >= x1 && position.x <= x2;
    bool CollisionY = position.y >= y1 && position.y <= y2;
    return CollisionX && CollisionY;
}

size_t TicTacToeScene::TDRootAction(int turn)
{
    size_t action = 0;
    if (m_Training) m_RootState = turn == 1 ? m_ModelMinimax.getNStepState(turn, s_Beta) : m_OpponentMinimax.getNStepState(turn, s_Beta);
    else action = turn == 1 && !m_Training ? action = m_ModelMinimax.getAction(turn) : action = m_OpponentMinimax.getNStepState(turn, s_Beta).Action;

    action = m_Training ? m_RootState.Action : action;

    return action;
}

size_t TicTacToeScene::TreestrapAction(int turn)
{
    size_t action = 0;
    if (m_Training) m_TreeData = turn == 1 ? m_ModelMinimax.treestrap(turn) : m_OpponentMinimax.treestrap(turn);
    else action = turn == 1 && !m_Training ? action = m_ModelMinimax.getAction(turn) : action = m_OpponentMinimax.getNStepState(turn, s_Beta).Action;

    action = m_Training ? m_TreeData.Action : action;

    return action;
}

size_t TicTacToeScene::ModelAction(int turn)
{
    Elysium::Matrix v;
    std::vector<float> values;
    for (size_t i = 0; i < m_Grid.Grid.size(); ++i)
    {
        if (m_Grid.isValid(i))
        {
            TicTacToeGrid next = m_Grid;
            next.Grid[i] = m_Turn;

            std::vector<float> state;

            switch (turn)
            {
            case 1:
                getState(next, state);
                m_TicTacToeModel.predict(Elysium::Matrix(state), v);
                break;
            case -1:
                getInvertedState(next, state);
                m_OpponentModels[m_OpponentIndex].predict(Elysium::Matrix(state), v);
            }
            values.push_back(v(0, 0));
        }
    }

    size_t action = 0;
    switch (turn)
    {
    case 1:
        switch (m_Policy)
        {
        case 0:
            action = getMaxAction(values);
            break;
        case 1:
            action = getSoftmaxAction(values, s_Beta);
            break;
        }
        break;
    case -1:
        switch (m_Opponent)
        {
        case 0:
            action = getRandomAction();
            break;
        case 1:
            action = getSoftmaxAction(values, s_Beta);
            break;
        case 2:
            action = getMaxAction(values);
            break;
        }
        break;
    }
    return action;
}

Elysium::Action TicTacToeScene::chooseAction(int turn)
{
    size_t action = 0;
    if (m_Algorithm == 2) action = TDRootAction(turn);
    else if (m_Algorithm == 3) action = TreestrapAction(turn);
    else action = ModelAction(turn);

    ELY_INFO("Action: {0}", action);

    return action;
}

size_t TicTacToeScene::getMaxAction(const std::vector<float>& values)
{
    std::vector<size_t> actions;
    for (size_t i = 0; i < m_Grid.Grid.size(); ++i)
    {
        if (m_Grid.isValid(i))
        {
            actions.push_back(i);
        }
    }

    float maxValue = -FLT_MAX;
    size_t action = 0;
    bool found = false;
    for (size_t i = 0; i < values.size(); ++i)
    {
        if (!found)
        {
            action = actions[i];
            found = true;
        }

        if (maxValue < values[i])
        {
            maxValue = values[i];
            action = actions[i];
        }
        printf("%zd : %f ", actions[i], values[i]);
    }
    printf("\n");

    return action;
}

size_t TicTacToeScene::getMinAction(const std::vector<float>& values)
{
    std::vector<size_t> actions;
    for (size_t i = 0; i < m_Grid.Grid.size(); ++i)
    {
        if (m_Grid.isValid(i))
        {
            actions.push_back(i);
        }
    }

    float minValue = FLT_MAX;
    size_t action = 0;
    bool found = false;
    for (size_t i = 0; i < values.size(); ++i)
    {
        if (!found)
        {
            action = actions[i];
            found = true;
        }

        if (minValue > values[i])
        {
            minValue = values[i];
            action = actions[i];
        }
        printf("%zd : %f ", actions[i], values[i]);
    }
    printf("\n");

    return action;
}

size_t TicTacToeScene::getRandomAction()
{
    size_t action = 0;

    Elysium::Random::InitInteger(0, 8);
    while (!m_Grid.isValid(action))
        action = (size_t)Elysium::Random::Integer();

    return action;
}

size_t TicTacToeScene::getSoftmaxAction(const std::vector<float>& stateValues, float beta)
{
    size_t j = 0;
    std::vector<size_t> actions;
    for (size_t i = 0; i < m_Grid.Grid.size(); ++i)
    {
        if (m_Grid.isValid(i))
        {
            actions.push_back(i);
            printf("%zd : %f ", i, stateValues[j++]);
        }
    }
    printf("\n");

    std::vector<float> distribution;

    Elysium::AI::softmax(stateValues, distribution, beta);
    float random = Elysium::Random::Float();

    bool found = false;

    float probability = 0.0f;
    size_t action = 0;
    for (size_t i = 0; i < distribution.size(); ++i)
    {
        probability += distribution[i];
        if (random < probability && !found)
        {
            action = actions[i];
            found = true;
        }

        printf("P(%zd) : %f ", actions[i], distribution[i]);
    }
    printf("\n");

    ELY_INFO("Random: {0}", random);

    return action;
}

size_t TicTacToeScene::getSoftminAction(const std::vector<float>& stateValues, float beta)
{
    size_t j = 0;
    std::vector<size_t> actions;
    for (size_t i = 0; i < m_Grid.Grid.size(); ++i)
    {
        if (m_Grid.isValid(i))
        {
            actions.push_back(i);
            printf("%zd : %f ", i, stateValues[j++]);
        }
    }
    printf("\n");

    std::vector<float> distribution;

    Elysium::AI::softmin(stateValues, distribution, beta);
    float random = Elysium::Random::Float();

    bool found = false;

    float probability = 0.0f;
    size_t action = 0;
    for (size_t i = 0; i < distribution.size(); ++i)
    {
        probability += distribution[i];
        if (random < probability && !found)
        {
            action = actions[i];
            found = true;
        }

        printf("P(%zd) : %f ", actions[i], distribution[i]);
    }
    printf("\n");

    ELY_INFO("Random: {0}", random);

    return action;
}

void TicTacToeScene::trainModel(const TicTacToeGrid& previous, Elysium::Action action, int turn)
{
    std::vector<float> prevState, state;
    getState(previous, prevState);
    getState(m_Grid, state);

    float reward = 0.0f;
    if (m_GameOver)
    {
        reward = turn == 1 ? s_WinReward : s_LossReward;
        reward = m_Tie ? s_TieReward : reward;
    }
    std::vector<float> data;
    if (m_Algorithm == 1) updateMonteCarloDataset(data, prevState, state, reward);
    else
    {
        data.insert(data.end(), prevState.begin(), prevState.end());
        data.insert(data.end(), state.begin(), state.end());
        data.push_back(reward);
    }

    switch (m_Algorithm)
    {
    case 0:
        TD(Elysium::Matrix(data));
        break;
    case 1:
        if (m_GameOver) MonteCarlo();
        break;
    case 2:
        TDRoot(Elysium::Matrix(data));
        break;
    case 3:
        Treestrap(Elysium::Matrix(data));
    }

    m_TicTacToeModel.report();

    m_DoneTraining = true;
}

void TicTacToeScene::updateMonteCarloDataset(std::vector<float>& data, const std::vector<float>& previous, const std::vector<float>& state, float reward)
{
    data.insert(data.end(), previous.begin(), previous.end());
    data.insert(data.end(), state.begin(), state.end());
    data.push_back(0.0f);

    if (m_GameOver)
    {
        data.insert(data.end(), state.begin(), state.end());
        data.insert(data.end(), state.begin(), state.end());
        data.push_back(reward);

        m_EpisodeData.appendRow(data);

        float discountedRewards = 0.0f;
        for (size_t i = 0; i < m_EpisodeData.getHeight(); ++i)
        {
            size_t index = m_EpisodeData.getHeight() - 1;
            discountedRewards += m_EpisodeData(index - i, s_DataLength - 1);
            m_EpisodeData(index - i, s_DataLength - 1) = discountedRewards;
            discountedRewards = s_DiscountFactor * discountedRewards;
        }
    }
}

void TicTacToeScene::TD(const Elysium::Matrix& data)
{
    Elysium::Matrix state = Elysium::Matrix::Slice(data, 0, 0, 0, 27);
    Elysium::Matrix nextState = Elysium::Matrix::Slice(data, 0, 0, 27, s_DataLength - 1);
    Elysium::Matrix reward = Elysium::Matrix::Slice(data, 0, 0, s_DataLength - 1, s_DataLength);

    Elysium::Matrix nextValue;
    m_TicTacToeModel.predict(nextState, nextValue);

    nextValue(0, 0) = 0.0f + s_DiscountFactor * nextValue.Values[0];

    m_TicTacToeModel.fit(state, nextValue, 1, 1);

    if (m_GameOver)
    {
        Elysium::Matrix target;
        m_TicTacToeModel.predict(nextState, target);

        target(0, 0) = reward.Values[0];

        m_TicTacToeModel.fit(nextState, target);

        ELY_INFO("Reward: {0}", target(0, 0));
    }
}

void TicTacToeScene::TDRoot(const Elysium::Matrix& data)
{
    Elysium::Matrix state = Elysium::Matrix::Slice(data, 0, 0, 0, 27);
    Elysium::Matrix nextState = Elysium::Matrix::Slice(data, 0, 0, 27, s_DataLength - 1);
    Elysium::Matrix reward = Elysium::Matrix::Slice(data, 0, 0, s_DataLength - 1, s_DataLength);

    std::vector<float> targetState;
    Elysium::Matrix targetValue;
    getState(m_RootState.TargetGrid, targetState);
    m_TicTacToeModel.predict(Elysium::Matrix(targetState), targetValue);

    float valueDiscount = (float)glm::pow(s_DiscountFactor, m_RootState.Depth);
    float r = 0.0f;
    switch (m_RootState.Result)
    {
    case -1:
        r = s_LossReward;
        break;
        break;
    case 1:
        r = s_WinReward;
        break;
    case 0:
        r = m_RootState.Terminal ? s_TieReward : 0.0f;
        break;
    }
    targetValue(0, 0) = m_RootState.Terminal ? valueDiscount * r : valueDiscount * targetValue(0, 0);

    Elysium::Matrix value, stateValue;
    m_TicTacToeModel.predict(state, value);
    m_TicTacToeModel.predict(nextState, stateValue);
    ELY_INFO("Value of Previous State: {0}", value(0, 0));
    ELY_INFO("TD Root Depth: {0} Value: {1} Result: {2}", m_RootState.Depth, targetValue(0, 0), m_RootState.Result);

    m_TicTacToeModel.fit(state, targetValue);

    if (m_GameOver)
    {
        Elysium::Matrix target;
        m_TicTacToeModel.predict(nextState, target);

        target(0, 0) = r;

        m_TicTacToeModel.fit(nextState, target);

        ELY_INFO("Value of Current State: {0}", stateValue(0, 0));
        ELY_INFO("Target: {0}", target(0, 0));
    }
}

void TicTacToeScene::Treestrap(const Elysium::Matrix& data)
{
    Elysium::Matrix currentState = Elysium::Matrix::Slice(data, 0, 0, 0, 27);
    Elysium::Matrix nextState = Elysium::Matrix::Slice(data, 0, 0, 27, s_DataLength - 1);
    Elysium::Matrix reward = Elysium::Matrix::Slice(data, 0, 0, s_DataLength - 1, s_DataLength);

    Elysium::Matrix states, targetStates, targets;
    for (TicTacToeMinimax::State s : m_TreeData.States)
    {
        std::vector<float> state, targetState;
        getState(s.Grid, state);
        states.appendRow(state);
        getState(s.TargetGrid, targetState);
        targetStates.appendRow(targetState);
    }

    m_TicTacToeModel.predict(targetStates, targets);

    for (size_t i = 0; i < m_TreeData.States.size(); ++i)
    {
        float valueDiscount = (float)glm::pow(s_DiscountFactor, 2 - m_TreeData.States[i].Depth);
        float r = 0.0f;
        switch (m_TreeData.States[i].Result)
        {
        case -1:
            r = s_LossReward;
            break;
            break;
        case 1:
            r = s_WinReward;
            break;
        case 0:
            r = m_TreeData.States[i].Terminal ? s_TieReward : 0.0f;
            break;
        }
        targets(i, 0) = m_TreeData.States[i].Terminal ? valueDiscount * r : valueDiscount * targets(i, 0);
    }

    Elysium::Matrix value, stateValue;
    m_TicTacToeModel.predict(currentState, value);
    m_TicTacToeModel.predict(nextState, stateValue);
    ELY_INFO("Value of Previous State: {0}", value(0, 0));
    ELY_INFO("Root Depth: {0} Value: {1} Result: {2}", m_TreeData.States.back().Depth, targets(m_TreeData.States.size() - 1, 0), m_TreeData.States.back().Result);

    m_TicTacToeModel.fit(states, targets);
}

void TicTacToeScene::MonteCarlo()
{
    Elysium::Matrix states = Elysium::Matrix::Slice(m_EpisodeData, 0, 0, 0, 27);
    Elysium::Matrix rewards = Elysium::Matrix::Slice(m_EpisodeData, 0, 0, s_DataLength - 1, s_DataLength);
    rewards.print();

    Elysium::Matrix targets;
    m_TicTacToeModel.predict(states, targets);

    for (size_t i = 0; i < m_EpisodeData.getHeight(); ++i)
    {
        targets(i, 0) = rewards.Values[i];
    }

    m_TicTacToeModel.fit(states, targets, 1, m_EpisodeData.getHeight());

    m_EpisodeData = Elysium::Matrix(0, s_DataLength);
}

void TicTacToeScene::getState(const TicTacToeGrid& grid, std::vector<float>& state)
{
    state.resize(27);
    for (size_t i = 0; i < grid.Grid.size(); ++i)
    {
        switch (grid.Grid[i])
        {
        case 1:
            state[i] = 1.0f;
            break;
        case -1:
            state[i + 9] = 1.0f;
            break;
        case 0:
            state[i + 18] = 1.0f;
            break;
        }
    }
}

void TicTacToeScene::getInvertedState(const TicTacToeGrid& grid, std::vector<float>& state)
{
    state.resize(27);
    for (size_t i = 0; i < m_Grid.Grid.size(); ++i)
    {
        switch (grid.Grid[i])
        {
        case 1:
            state[i + 9] = 1.0f;
            break;
        case -1:
            state[i] = 1.0f;
            break;
        case 0:
            state[i + 18] = 1.0f;
            break;
        }
    }
}

void TicTacToeScene::updateOpponent()
{
    auto model = m_TicTacToeModel.getLayers();
    auto target = m_OpponentModels[m_OpponentUpdateIndex].getLayers();
    for (size_t i = 0; i < model.size(); ++i)
    {
        target[i]->Weights.Values = model[i]->Weights.Values;
        target[i]->Biases = model[i]->Biases;
    }
}

void TicTacToeScene::updateTarget()
{
    auto model = m_TicTacToeModel.getLayers();
    auto target = m_TargetModel.getLayers();
    for (size_t i = 0; i < model.size(); ++i)
    {
        target[i]->Weights.Values = model[i]->Weights.Values;
        target[i]->Biases = model[i]->Biases;
    }
}

void TicTacToeScene::modelGenerator(std::vector<TicTacToeMinimax::State>& states, const TicTacToeGrid& grid, int turn, size_t depth)
{
    bool printedValues = false;
    for (size_t i = 0; i < m_Grid.Grid.size(); ++i)
    {
        TicTacToeGrid next = grid;
        if (next.isValid(i))
        {
            next.Grid[i] = turn;

            std::vector<float> state;
            getState(next, state);

            Elysium::Matrix values;
            m_TicTacToeModel.predict(Elysium::Matrix(state), values);

            if (grid.isWinningMove(i, turn) || next.isFilled())
            {
                int result = grid.isWinningMove(i, turn) ? turn : 0;
                states.emplace_back(values(0, 0), i, depth, true, result, next);

                printf("%zd : %f ", i, values(0, 0));
                printedValues = true;
                continue;
            }
            else if (depth == TicTacToeMinimax::s_Depth)
            {
                states.emplace_back(values(0, 0), i, depth, false, 0, next);

                printf("%zd : %f ", i, values(0, 0));
                printedValues = true;
                continue;
            }
            states.emplace_back(0.0f, i, depth, false, 0, next);
        }
    }
    if (printedValues) printf("\n");
}

void TicTacToeScene::opponentGenerator(std::vector<TicTacToeMinimax::State>& states, const TicTacToeGrid& grid, int turn, size_t depth)
{
    bool printedValues = false;
    for (size_t i = 0; i < m_Grid.Grid.size(); ++i)
    {
        TicTacToeGrid next = grid;
        if (next.isValid(i))
        {
            next.Grid[i] = turn;

            std::vector<float> state;
            getInvertedState(next, state);

            Elysium::Matrix values;
            m_OpponentModels[m_OpponentIndex].predict(Elysium::Matrix(state), values);

            if (grid.isWinningMove(i, turn) || next.isFilled())
            {
                int result = grid.isWinningMove(i, turn) ? turn : 0;
                states.emplace_back(values(0, 0), i, depth, true, result, next);

                printf("%zd : %f ", i, values(0, 0));
                printedValues = true;
                continue;
            }
            else if (depth == TicTacToeMinimax::s_Depth)
            {
                states.emplace_back(values(0, 0), i, depth, false, 0, next);

                printf("%zd : %f ", i, values(0, 0));
                printedValues = true;
                continue;
            }
            states.emplace_back(0.0f, i, depth, false, 0, next);
        }
    }
    if (printedValues) printf("\n");
}

void TicTacToeScene::updateModels()
{
    m_Grid.printGrid();

    m_GamesPlayed++;
    m_Wins += m_PreviousTurn == 1 && !m_Tie ? 1 : 0;
    m_Draws += m_Tie ? 1 : 0;
    m_WinRate = m_GamesPlayed > 0 ? (float)m_Wins / (float)m_GamesPlayed : 0.0f;
    m_DrawRate = m_GamesPlayed > 0 ? (float)m_Draws / (float)m_GamesPlayed : 0.0f;

    if (!m_Training)
    {
        m_ModelWins[m_OpponentIndex] += m_PreviousTurn == 1 && !m_Tie ? 1 : 0;
        m_ModelDraws[m_OpponentIndex] += m_Tie ? 1 : 0;
        m_ModelGames[m_OpponentIndex]++;
    }

    if (m_Training) m_OpponentIndex = ++m_OpponentIndex < 4 ? m_OpponentIndex : 0;
    else if (!m_Training && m_GamesPlayed % 25 == 0) m_OpponentIndex = ++m_OpponentIndex < 4 ? m_OpponentIndex : 0;

    if (m_GamesPlayed % s_UpdateOpponent == 0)
    {
        if (m_Training)
        {
            m_ModelWins = { 0 };
            m_ModelDraws = { 0 };
            m_ModelGames = { 0 };
        }

        if (!m_Training)
        {
            bool update = true;
            size_t index = 0;
            size_t highestWins = 0;
            for (size_t i = 0; i < 4; ++i)
            {
                if (m_ModelWins[i] > highestWins)
                {
                    index = i;
                    highestWins = (size_t)m_ModelWins[i];
                }

                if (m_ModelWins[i] + m_ModelDraws[i] < m_PositiveThreshold && m_ModelWins[i] < m_WinThreshold) update = false;
            }
            m_OpponentUpdateIndex = index;

            if (update)
            {
                updateOpponent();
                m_OpponentUpdates++;
                m_OpponentUpdated[m_OpponentUpdateIndex] = true;

                if (m_OpponentUpdated[0] && m_OpponentUpdated[1] && m_OpponentUpdated[2] && m_OpponentUpdated[3])
                {
                    m_PositiveThreshold++;
                    m_WinThreshold++;
                    m_OpponentUpdated.fill(false);
                }
            }

            m_Pause = m_Wins + m_Draws >= s_RateThreshold;
        }

        m_Policy = m_Policy == 0 ? 1 : 0;
        m_Training = !m_Training;

        m_GamesPlayed = 0;
        m_Wins = 0;
        m_Draws = 0;

        m_OpponentIndex = 0;
    }

    if (m_DoneSaving)
    {
        std::thread save(&TicTacToeScene::saveModels, this);
        save.detach();
    }

    m_MoveCount = 0;
}

void TicTacToeScene::onUpdate(Elysium::Timestep ts)
{
    if (!m_Pause && m_DoneTraining)
    {
        if (m_GameOver && m_MoveCooldown >= 0.0f)
        {
            if (m_Tie)
            {
                m_DrawCount++;
                Elysium::Random::InitInteger(1, 2);
                m_Turn = Elysium::Random::Integer() == 1 ? -1 : 1;
            }
            m_MoveCooldown = s_Cooldown;
            m_Coins.fill({ 0.0f, 0.0f });
            m_CoinsTextureIndexes.fill(0);

            m_CoinIndex = 0;

            if (m_TrainModel) updateModels();

            m_GameOver = false;
            m_Grid.clear();
        }

        if (m_PlayAgainstMinimax && !m_PlayModel && !m_TrainModel && m_MoveCooldown >= 0.0f)
        {
            if (m_Turn == 1)
            {
                size_t action = m_Minimax.getAction(m_Turn);

                addAction(getPosition((Elysium::Action)action), action);
            }
        }

        if (!m_PlayAgainstMinimax && !m_PlayModel && m_TrainModel && m_MoveCooldown >= 0.0f)
        {
            m_Grid.printGrid();

            TicTacToeGrid state = m_Grid;
            ELY_INFO("Turn: {0}", m_Turn);

            size_t action = chooseAction(m_Turn);

            int turn = m_Turn;
            addAction(getPosition(action), action);
            m_PreviousTurn = turn;

            if (m_Policy > 0 && m_Training)
            {
                if (++m_TargetUpdate % s_UpdateTarget == 0) updateTarget();

                m_DoneTraining = false;
                std::thread trainingThread(&TicTacToeScene::trainModel, this, state, action , turn);
                trainingThread.detach();
            }
        }

        if (!m_PlayAgainstMinimax && m_PlayModel && !m_TrainModel && m_MoveCooldown >= 0.0f)
        {
            if (m_Turn == 1)
            {
                size_t action = m_ModelMinimax.getAction(m_Turn);

                ELY_INFO("Action: {0}", action);

                addAction(getPosition(action), action);
            }
        }
    }

    if (m_MoveCooldown < 0.0f) m_MoveCooldown += ts;

    Elysium::Renderer::beginScene(m_Camera);
    float position = 3.0f;
    float size = 18.0f;
    float pos = 6.0f;
    Elysium::Renderer::drawQuad({ 0.0f, position }, { size, 2.0f }, { 0.0f, 0.0f, 1.0f, 1.0f });
    Elysium::Renderer::drawQuad({ 0.0f, -position }, { size, 2.0f }, { 0.0f, 0.0f, 1.0f, 1.0f });
    Elysium::Renderer::drawQuad({ position, 0.0f }, { 2.0f, size }, { 0.0f, 0.0f, 1.0f, 1.0f });
    Elysium::Renderer::drawQuad({ -position, 0.0f }, { 2.0f, size }, { 0.0f, 0.0f, 1.0f, 1.0f });

    for (size_t i = 0; i < m_CoinIndex; i++)
    {
        Elysium::Renderer::drawQuad(m_Coins[i], { 3.0f, 3.0f }, m_CoinTextures[m_CoinsTextureIndexes[i] - 1]);
    }
    Elysium::Renderer::endScene();

    ImGui::Begin("Tic-Tac-Toe");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("O: %d : X %d, Draws: %d", m_OScore, m_XScore, m_DrawCount);
    if (m_TrainModel)
    {
        ImGui::Text("Total Games: %d", m_OScore + m_XScore + m_DrawCount);
        if (m_GamesPlayed > 0) ImGui::Text("Epoch Rate : %f", (float)(m_Wins + m_Draws) / (float)m_GamesPlayed);
        ImGui::Text("Games Played: %d, Win rate: %f, Draw rate: %f", m_GamesPlayed, m_WinRate, m_DrawRate);
        const char* label = m_Training ? "Training" : "Testing";
        ImGui::Text("Phase: %s", label);
        ImGui::Text("Update Index: %d, Opponent Updates: %d, Thresholds: %d %d", m_OpponentUpdateIndex, m_OpponentUpdates, m_PositiveThreshold, m_WinThreshold);

        ImGui::Text("Model vs Opponents Games");
        ImGui::Text("0: %u, 1: %u, 2: %u, 3: %u", m_ModelGames[0], m_ModelGames[1], m_ModelGames[2], m_ModelGames[3]);
        ImGui::Text("Model vs Opponents Wins");
        ImGui::Text("0: %u, 1: %u, 2: %u, 3: %u", m_ModelWins[0], m_ModelWins[1], m_ModelWins[2], m_ModelWins[3]);
        ImGui::Text("Model vs Opponents Draws");
        ImGui::Text("0: %u, 1: %u, 2: %u, 3: %u", m_ModelDraws[0], m_ModelDraws[1], m_ModelDraws[2], m_ModelDraws[3]);
    }
    //ImGui::Checkbox("Train Model", &m_TrainModel);
    ImGui::Checkbox("Play Model", &m_PlayModel);
    ImGui::Checkbox("Play Minimax", &m_PlayAgainstMinimax);
    ImGui::Checkbox("Pause Game", &m_Pause);
    ImGui::End();

    Elysium::Renderer::resetStats();
}

void TicTacToeScene::onEvent(Elysium::Event& event)
{
    Elysium::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Elysium::MouseButtonPressedEvent>(BIND_EVENT_FUNCTION(TicTacToeScene::onMousePressedEvent));
    dispatcher.Dispatch<Elysium::WindowResizeEvent>(BIND_EVENT_FUNCTION(TicTacToeScene::onWindowResizeEvent));
}

void TicTacToeScene::saveModels()
{
    m_DoneSaving = false;
    m_TicTacToeModel.save("res/AI/Tic-Tac-Toe/tictactoe-model");

    for (size_t i = 0; i < 4; ++i)
    {
        std::string path = "res/AI/Tic-Tac-Toe/opponent-model";
        std::string name = path + '-' + std::to_string(i);
        m_OpponentModels[i].save(name.c_str());
    }
    m_DoneSaving = true;
}

bool TicTacToeScene::onMousePressedEvent(Elysium::MouseButtonPressedEvent& event)
{
    if (!m_GameOver && m_Turn == -1 && m_MoveCooldown >= 0.0f)
    {
        auto mousePosition = Elysium::Input::getMousePosition();
        auto width = Elysium::Application::Get().getWindow().getWidth();
        auto height = Elysium::Application::Get().getWindow().getHeight();

        auto cursorPosition = m_Camera.getScreenToWorldPosition(width, height, mousePosition);

        Elysium::Vector2 position = { 0.0f, 0.0f };
        size_t index = 0;
        bool validClick = false;
        if (isWithinBounds(cursorPosition, -8.5f, -8.5f, -3.5f, -3.5f))
        {
            position = { -6.0f, -6.0f };
            index = 0;
            validClick = true;
        }
        else if (isWithinBounds(cursorPosition, -2.5f, -8.5f, 2.5f, -3.5f))
        {
            position = { 0.0f, -6.0f };
            index = 1;
            validClick = true;
        }
        else if (isWithinBounds(cursorPosition, 3.5f, -8.5f, 8.5f, -3.5f))
        {
            position = { 6.0f, -6.0f };
            index = 2;
            validClick = true;
        }
        else if (isWithinBounds(cursorPosition, -8.5f, -2.5f, -3.5f, 2.5f))
        {
            position = { -6.0f, 0.0f };
            index = 3;
            validClick = true;
        }
        else if (isWithinBounds(cursorPosition, -2.5f, -2.5f, 2.5f, 2.5f))
        {
            position = { 0.0f, 0.0f };
            index = 4;
            validClick = true;
        }
        else if (isWithinBounds(cursorPosition, 3.5f, -2.5f, 8.5f, 2.5f))
        {
            position = { 6.0f, 0.0f };
            index = 5;
            validClick = true;
        }
        else if (isWithinBounds(cursorPosition, -8.5f, 3.5f, -3.5f, 8.5f))
        {
            position = { -6.0f, 6.0f };
            index = 6;
            validClick = true;
        }
        else if (isWithinBounds(cursorPosition, -2.5f, 3.5f, 2.5f, 8.5f))
        {
            position = { 0.0f, 6.0f };
            index = 7;
            validClick = true;
        }
        else if (isWithinBounds(cursorPosition, 3.5f, 3.5f, 8.5f, 8.5f))
        {
            position = { 6.0f, 6.0f };
            index = 8;
            validClick = true;
        }

        if (validClick)
            addAction(position, index);
    }
    return false;
}

bool TicTacToeScene::onWindowResizeEvent(Elysium::WindowResizeEvent& event)
{
    unsigned int width = event.getWidth();
    unsigned int height = event.getHeight();
    if (width > 0 && height > 0)
        m_Camera.setProjection(-m_Height * (float)(width / height), m_Height * (float)(width / height),
            -m_Height * 0.5f, m_Height * 0.5f);
    return false;
}