#include "TicTacToeScene.h"

#include "Game/Systems.h"

TicTacToeScene::TicTacToeScene(unsigned int width, unsigned int height) : 
    Elysium::Scene("Tic-Tac-Toe"),
    m_Camera(-m_Height * (float)(width / height), m_Height * (float)(width / height), -m_Height * 0.5f, m_Height * 0.5f),
    m_SpriteSheet("res/texture/platformPack_tilesheet.png"),
    m_Minimax(&m_Grid),
    m_ModelMinimax(&m_Grid),
    m_TicTacToeModel(s_InputLength),
    m_Dataset(0, s_DataLength)
{
    m_CoinTextures[0] = m_SpriteSheet.getTextureData();
    m_CoinTextures[0].subtextureCoordinates({ 10, 6 }, { 128, 128 });

    m_CoinTextures[1] = m_SpriteSheet.getTextureData();
    m_CoinTextures[1].subtextureCoordinates({ 11, 5 }, { 128, 128 });

    m_Camera.setPosition({ 0.0f, 0.0f, 0.0f });

    m_TicTacToeModel.add(new Elysium::Dense(243, Elysium::AI::Activation::LEAKY_RELU, Elysium::AI::Initializer::HE));
    m_TicTacToeModel.add(new Elysium::Dense(9, Elysium::AI::Activation::LINEAR));
    m_TicTacToeModel.LossFunction = Elysium::AI::Loss::MEAN_SQUARED;
    m_TicTacToeModel.LearningRate = 0.00001f;

    if (m_TicTacToeModel.load("res/AI/tictactoe-model"))
        loadDataset("res/AI/tictactoe-dataset.csv");
    m_TicTacToeModel.summary();
}

TicTacToeScene::~TicTacToeScene()
{
    m_TicTacToeModel.save("res/AI/tictactoe-model");
    saveDataset("res/AI/tictactoe-dataset.csv");
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

        m_RedWon = false;

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
                m_BlueScore++;
                break;
            case 1:
                m_RedScore++;
                m_RedWon = true;
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

void TicTacToeScene::loadDataset(const char* filepath)
{
    std::ifstream file(filepath);
    if (file.is_open())
    {
        std::string line;
        while (getline(file, line))
        {
            if (!line.empty())
            {
                std::vector<float> row(s_DataLength);
                float reward = 0.0f;
                float action = 0.0f;
                int terminal = 0;
                sscanf_s(line.c_str(),
                    "%f, %f, %f, %f, %f, %f, %f, %f, %f \
                    %f, %f, %f, %f, %f, %f, %f, %f, %f, \
                    %f, %f, %f, %f, %f, %f, %f, %f, %f, \
                    %f, %f, %f, %f, %f, %f, %f, %f, %f, \
                    %f, %f, %f, %f, %f, %f, %f, %f, %f, \
                    %f, %f, %f, %f, %f, %f, %f, %f, %f, \
                    A: %f, R: %f, T: %d",
                    &row[0], &row[1], &row[2], &row[3], &row[4], &row[5], &row[6], &row[7], &row[8],
                    &row[9], &row[10], &row[11], &row[12], &row[13], &row[14], &row[15], &row[16], &row[17],
                    &row[18], &row[19], &row[20], &row[21], &row[22], &row[23], &row[24], &row[25], &row[26],
                    &row[27], &row[28], &row[29], &row[30], &row[31], &row[32], &row[33], &row[34], &row[35],
                    &row[36], &row[37], &row[38], &row[39], &row[40], &row[41], &row[42], &row[43], &row[44],
                    &row[45], &row[46], &row[47], &row[48], &row[49], &row[50], &row[51], &row[52], &row[53],
                    &action, &reward, &terminal);
                row[54] = action;
                row[55] = reward;
                m_TerminalStates.push_back(terminal);
                m_Dataset.appendRow(row);
            }
        }
        file.close();
    }
}

void TicTacToeScene::saveDataset(const char* filepath)
{
    std::ofstream file(filepath);
    if (file.is_open())
    {
        for (size_t i = 0; i < m_Dataset.getHeight(); ++i)
        {
            for (size_t j = 0; j < m_Dataset.getWidth(); ++j)
            {
                switch (j)
                {
                case 54:
                    file << "A: " << m_Dataset(i, j) << ", ";
                    break;
                case 55:
                    file << "R: " << m_Dataset(i, j) << ", "  << "T: " << m_TerminalStates[i] << '\n';
                    break;
                default:
                    file << m_Dataset(i, j) << ", ";
                }
            }
        }
    }
    file.close();
}

Elysium::Action TicTacToeScene::chooseAction()
{
    Elysium::Matrix Q;
    std::vector<float> data;
    getState(m_Grid, data);
    Elysium::Matrix state(data);
    m_TicTacToeModel.predict(state, Q);

    std::vector<float> values, distribution;
    std::vector<size_t> actions;
    for (size_t i = 0; i < 9; ++i)
    {
        if (m_Grid.isValid(i))
        {
            values.push_back(Q.Values[i]);
            actions.push_back(i);
        }
    }

    Elysium::AI::softmax(values, distribution);
    float random = Elysium::Random::Float();

    bool found = false;
    float probability = 0.0f;
    size_t action = 0;

    size_t last = distribution.size() - 1;
    for (size_t i = 0; i < distribution.size(); ++i)
    {
        probability += distribution[i];
        if (random < probability && !found)
        {
            action = actions[i];
            found = true;
        }

        if (i < last)
            printf("%zd : %f, ", actions[i], distribution[i]);
    }
    printf("%zu : %f\n", actions[last], distribution[last]);
    fflush(stdout);

    Elysium::Matrix(values).print();
    ELY_INFO("Random: {0}, Action: {1}", random, action);

    return action;
}

void TicTacToeScene::trainModel(const TicTacToeGrid& previous, Elysium::Action action)
{
    m_DoneTraining = false;
    std::vector<float> s, s_prime, data;
    getState(previous, s);
    getState(m_Grid, s_prime);
    float reward = 0.0f;
    if (m_GameOver)
    {
        reward = m_RedWon ? 1.0f : -1.0f;
        reward = m_Tie ? 0.0f : reward;
    }
    data.insert(data.end(), s.begin(), s.end());
    data.insert(data.end(), s_prime.begin(), s_prime.end());
    data.push_back(float(action));
    data.push_back(reward);
    m_TerminalStates.push_back((int)m_GameOver);
    m_Dataset.appendRow(data);

    constexpr size_t batchSize = 10;
    if (m_Dataset.getHeight() > batchSize * 2)
    {
        constexpr size_t memorySize = 10000;
        if (m_Dataset.getHeight() > memorySize)
            m_Dataset = Elysium::Matrix::Slice(m_Dataset, m_Dataset.getHeight() - memorySize, 0, 0, 0);


        size_t i = 0;
        Elysium::Random::InitInteger(0, (int)(m_Dataset.getHeight() - 1));
        Elysium::Matrix dataset(0, s_DataLength);
        std::vector<int> terminals(10);
        while (i < batchSize)
        {
            size_t row = (size_t)Elysium::Random::Integer();
            dataset.appendRow(Elysium::Matrix::Slice(m_Dataset, row, row + 1, 0, 0).Values);
            terminals[i] = m_TerminalStates[row];
            i++;
        }

        Elysium::Matrix states = Elysium::Matrix::Slice(dataset, 0, 0, 0, 27);
        Elysium::Matrix nextStates = Elysium::Matrix::Slice(dataset, 0, 0, 27, 54);
        Elysium::Matrix actions = Elysium::Matrix::Slice(dataset, 0, 0, 54, 55);
        Elysium::Matrix rewards = Elysium::Matrix::Slice(dataset, 0, 0, 55, 56);

        Elysium::Matrix targets;
        m_TicTacToeModel.predict(states, targets);

        Elysium::Matrix nextQValues;
        m_TicTacToeModel.predict(nextStates, nextQValues);

        std::vector<float> maxQValues;
        maxQValues.reserve(batchSize);
        for (size_t i = 0; i < batchSize; ++i)
        {
            float max = nextQValues(i, 0);
            for (size_t j = 1; j < nextQValues.getWidth(); ++j)
                max = glm::max(max, nextQValues(i, j));
            maxQValues.emplace_back(max);
        }

        std::vector<float> y;

        float discountFactor = 0.95f;
        for (size_t i = 0; i < batchSize; ++i)
        {
            float target = terminals[i] ? rewards.Values[i] : rewards.Values[i] + discountFactor * maxQValues[i];
            targets(i, (size_t)actions.Values[i]) = target;
            y.push_back(target);
        }

        m_TicTacToeModel.fit(states, targets, 1, batchSize);
        m_TicTacToeModel.report();
    }
    m_DoneTraining = true;
}

void TicTacToeScene::getState(const TicTacToeGrid& grid, std::vector<float>& state)
{
    state.resize(27);
    for (size_t i = 0; i < grid.Grid.size(); ++i)
    {
        switch (grid.Grid[i])
        {
        case 0:
            state[i + 18] = 1.0f;
            break;
        case -1:
            state[i + 9] = 1.0f;
            break;
        case 1:
            state[i + 0] = 1.0f;
            break;
        }
    }
}

void TicTacToeScene::getInvertedState(std::vector<float>& state)
{
    for (size_t i = 0; i < 9; ++i)
    {
        float tmp = state[i];
        state[i] = state[i + 9];
        state[i + 9] = tmp;
    }
}

void TicTacToeScene::onUpdate(Elysium::Timestep ts)
{
    if (!m_Pause)
    {
        if (m_GameOver && m_DoneTraining && m_MoveCooldown >= 0.0f)
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
            m_GameOver = false;

            m_Grid.clear();

            m_TicTacToeModel.save("res/AI/tictactoe-model");
            saveDataset("res/AI/tictactoe-dataset.csv");
        }

        if (m_PlayAgainstMinimax && m_MoveCooldown >= 0.0f)
        {
            if (m_Turn == 1)
            {
                TicTacToeGrid s = m_Grid;
                m_Minimax.Minimax = -1;
                m_Minimax.Opponent = 1;
                size_t action = m_Minimax.playAction();

                addAction(getPosition((Elysium::Action)action), action);
            }
        }

        if (m_DoneTraining)
        {
            if (m_TrainModel && m_MoveCooldown >= 0.0f)
            {
                if (m_Turn == 1)
                {
                    std::vector<size_t> actions;
                    std::vector<float> stateValues;

                    TicTacToeGrid s = m_Grid;

                    size_t action = chooseAction();

                    addAction(getPosition(action), action);
                    std::thread trainingThread(&TicTacToeScene::trainModel, this, s, action);
                    trainingThread.detach();
                }
            }

            if (m_PlayAgainstModel && m_MoveCooldown >= 0.0f)
            {
                if (m_Turn == -1)
                {
                    TicTacToeGrid s = m_Grid;
                    /*
                    std::vector<size_t> actions;
                    std::vector<float> stateValues;

                    TicTacToeGrid s = m_Grid;

                    //m_ModelMinimax.Minimax = 1;
                    //m_ModelMinimax.Opponent = -1;
                    //size_t action = m_ModelMinimax.playAction();

                    size_t action = 0;
                    for (size_t i = 0; i < 9; ++i)
                    {
                        if (m_Grid.isValid(i))
                        {
                            action = i;
                            TicTacToeGrid tmp = m_Grid;
                            tmp.Grid[i] = m_Turn;

                            Elysium::Matrix value;
                            std::vector<float> s_prime;
                            getState(tmp, s_prime);
                            getInvertedState(s_prime);
                            m_TicTacToeModel.predict(Elysium::Matrix(s_prime), value);

                            actions.push_back(i);
                            stateValues.push_back(value[{0, 0}]);
                        }
                    }
                    std::vector<float> distribution;
                    Elysium::AI::softmax(stateValues, distribution);
                    //float random = Elysium::Random::Float();
                    */

                    //bool found = false;
                    //float probability = 0.0f;
                    /*
                    float maxValue = distribution[0];
                    for (size_t i = 0; i < distribution.size(); ++i)
                    {
                        if (maxValue < distribution[i])
                        {
                            maxValue = distribution[i];
                            action = actions[i];
                        }
                        /*
                        probability += distribution[i];
                        if (random < probability && !found)
                        {
                            action = actions[i];
                            found = true;
                        }
                        */
                    //}

                    Elysium::Random::InitInteger(0, 8);
                    size_t action = (size_t)Elysium::Random::Integer();
                    while (!m_Grid.isValid(action))
                        action = (size_t)Elysium::Random::Integer();

                    addAction(getPosition(action), action);
                    std::thread trainingThread(&TicTacToeScene::trainModel, this, s, action);
                    trainingThread.detach();
                }
            }
        }
    }

    if (m_MoveCooldown < 0.0f)
        m_MoveCooldown += ts;

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
        Elysium::Renderer::drawQuad(m_Coins[i], { 2.0f, 2.0f }, m_CoinTextures[m_CoinsTextureIndexes[i] - 1]);
    }
    Elysium::Renderer::endScene();

    ImGui::Begin("Tic-Tac-Toe");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Number of Draw Calls: %d", Elysium::Renderer::getStats().DrawCount);
    ImGui::Text("Number of Quads: %d", Elysium::Renderer::getStats().QuadCount);
    ImGui::Text("Red: %d : Blue %d, Draws: %d", m_RedScore, m_BlueScore, m_DrawCount);
    ImGui::Checkbox("Train Model", &m_TrainModel);
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

bool TicTacToeScene::onMousePressedEvent(Elysium::MouseButtonPressedEvent& event)
{
    if (!m_GameOver && m_Turn == 1 && m_MoveCooldown >= 0.0f)
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
            addAction(position, index);;
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