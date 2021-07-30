#include "TicTacToeScene.h"

#include "Game/Systems.h"

TicTacToeScene::TicTacToeScene(unsigned int width, unsigned int height) : 
    Elysium::Scene("Tic-Tac-Toe"),
    m_Camera(-m_Height * (float)(width / height), m_Height * (float)(width / height), -m_Height * 0.5f, m_Height * 0.5f),
    m_SpriteSheet("res/texture/platformPack_tilesheet.png"),
    m_Minimax(&m_Grid),
    m_ModelMinimax(&m_Grid),
    m_TicTacToeModel(9),
    m_Dataset(0, 19),
    m_EpisodeData(0, 19)
{
    m_CoinTextures[0] = m_SpriteSheet.getTextureData();
    m_CoinTextures[0].subtextureCoordinates({ 10, 6 }, { 128, 128 });

    m_CoinTextures[1] = m_SpriteSheet.getTextureData();
    m_CoinTextures[1].subtextureCoordinates({ 11, 5 }, { 128, 128 });

    m_Camera.setPosition({ 0.0f, 0.0f, 0.0f });

    m_TicTacToeModel.add(new Elysium::Dense(18, Elysium::AI::Activation::LEAKY_RELU));
    m_TicTacToeModel.add(new Elysium::Dense(18, Elysium::AI::Activation::LEAKY_RELU));
    m_TicTacToeModel.add(new Elysium::Dense(1, Elysium::AI::Activation::LINEAR));
    m_TicTacToeModel.LossFunction = Elysium::AI::Loss::MEAN_SQUARED;
    m_TicTacToeModel.LearningRate = 0.00000001f;

    m_ModelMinimax.ValueFunction = std::bind(&TicTacToeScene::evaluateState, this, std::placeholders::_1);
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
        std::vector<std::vector<float>> dataset;
        std::string line;
        while (getline(file, line))
        {
            if (!line.empty())
            {
                std::vector<float> row(19);
                float expectedReturn = 0.0f;
                sscanf_s(line.c_str(),
                    "%f, %f, %f, %f, %f, %f, %f, %f, %f | G: %f | %f, %f, %f, %f, %f, %f, %f, %f, %f",
                    &row[0], &row[1], &row[2], &row[3], &row[4], &row[5], &row[6], &row[7], &row[8],
                    &expectedReturn,
                    &row[10], &row[11], &row[12], &row[13], &row[14], &row[15], &row[16], &row[17], &row[18]);
                row[9] = expectedReturn;
                dataset.push_back(row);
            }
        }
        file.close();

        if (!dataset.empty())
            m_Dataset = Elysium::Matrix(dataset);
    }
}

void TicTacToeScene::saveDataset(const char* filepath)
{
    std::ofstream file(filepath);
    if (file.is_open())
    {
        const size_t lastColumn = m_Dataset.getWidth() - 1;
        for (size_t i = 0; i < m_Dataset.getHeight(); ++i)
        {
            for (size_t j = 0; j < lastColumn; ++j)
            {
                switch (j)
                {
                case 8:
                    file << m_Dataset[{i, j}] << " | ";
                    break;
                case 9:
                    file << "G: " << m_Dataset[{i, j}] << " | ";
                    break;
                default:
                    file << m_Dataset[{i, j}] << ", ";
                }
            }
            file << m_Dataset[{i, lastColumn}] << "\n";
        }
    }
    file.close();
}

float TicTacToeScene::evaluateState(const TicTacToeGrid& grid)
{
    Elysium::Matrix value;
    std::vector<float> data;
    grid.getStateOfGrid(data);
    Elysium::Matrix state(data);
    m_TicTacToeModel.predict(state, value);

    return value[{0, 0}];
}

void TicTacToeScene::trainModel(const TicTacToeGrid& previous)
{
    m_DoneTraining = false;
    std::vector<float> data;
    previous.getStateOfGrid(data);
    data.push_back(0.0f);
    m_Grid.getStateOfGrid(data);
    m_EpisodeData.appendRow(data);

    float reward = 0.0f;
    float discountFactor = 0.9f;
    if (m_GameOver)
    {
        reward = m_RedWon ? 1.0f : -1.0f;
        reward = m_Tie ? 0.0f : reward;
        if (glm::abs(reward) > 0.0f)
        {
            float expectedReturn = reward;
            for (int i = (int)m_EpisodeData.getHeight() - 1; i >= 0; --i)
            {
                m_EpisodeData[{(size_t)i, 9}] = expectedReturn;
                expectedReturn *= discountFactor;
            }
        }
        m_Dataset = Elysium::Matrix::Concatenate(m_Dataset, m_EpisodeData);
        m_EpisodeData = Elysium::Matrix(0, 19);
    }

    constexpr size_t batchSize = 10;
    if (m_Dataset.getHeight() > batchSize * 2)
    {
        constexpr size_t memorySize = 10000;
        if (m_Dataset.getHeight() > memorySize)
            m_Dataset = Elysium::Matrix::Slice(m_Dataset, m_Dataset.getHeight() - memorySize, 0, 0, 0);

        size_t start, end = 0;
        start = (size_t)Elysium::Random::Integer(0, (int)m_Dataset.getHeight());
        if (start >= m_Dataset.getHeight() - 10)
        {
            end = start;
            start = end - 10;
        }
        else
        {
            end = start + 10;
        }
        Elysium::Matrix dataset = Elysium::Matrix::Slice(m_Dataset, start, end, 0, 0);

        Elysium::Matrix states = Elysium::Matrix::Slice(dataset, 0, 0, 0, 9);
        Elysium::Matrix expectedReturns = Elysium::Matrix::Slice(dataset, 0, 0, 9, 10);

        m_TicTacToeModel.fit(states, expectedReturns, 1, batchSize);
        m_TicTacToeModel.report();
    }
    m_DoneTraining = true;
}

void TicTacToeScene::getInvertedState(std::vector<float>& state)
{
    for (size_t i = 0; i < state.size(); ++i)
        state[i] = -state[i];
}

void TicTacToeScene::onUpdate(Elysium::Timestep ts)
{
    if (!m_Pause)
    {
        if (m_GameOver && m_MoveCooldown >= 0.0f)
        {
            if (m_Tie)
            {
                m_DrawCount++;
                m_Turn = Elysium::Random::Integer(1, 2) == 1 ? -1 : 1;
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

                    size_t action = 0;
                    for (size_t i = 0; i < 9; ++i)
                    {
                        if (m_Grid.isValid(i))
                        {
                            action = i;
                            TicTacToeGrid temp = m_Grid;
                            temp.Grid[i] = m_Turn;

                            Elysium::Matrix value;
                            std::vector<float> s_prime;
                            temp.getStateOfGrid(s_prime);
                            m_TicTacToeModel.predict(Elysium::Matrix(s_prime), value);

                            actions.push_back(i);
                            stateValues.push_back(value[{0, 0}]);
                        }
                    }
                    std::vector<float> distribution;
                    Elysium::AI::softmax(stateValues, distribution);
                    float random = Elysium::Random::Float();

                    bool found = false;
                    float probability = 0.0f;
                    for (size_t i = 0; i < distribution.size(); ++i)
                    {
                        probability += distribution[i];
                        if (random < probability && !found)
                        {
                            action = actions[i];
                            found = true;
                        }

                        if (i < distribution.size() - 1)
                            printf("%zd : %f, ", actions[i], distribution[i]);
                    }
                    printf("%zd : %f\n", actions[distribution.size() - 1], distribution[distribution.size() - 1]);
                    fflush(stdout);

                    Elysium::Matrix(stateValues).print();
                    ELY_INFO("Random: {0}, Action: {1}", random, action);

                    addAction(getPosition(action), action);
                    std::thread trainingThread(&TicTacToeScene::trainModel, this, s);
                    trainingThread.detach();
                }
            }

            if (m_PlayAgainstModel && m_MoveCooldown >= 0.0f)
            {
                if (m_Turn == -1)
                {
                    TicTacToeGrid s = m_Grid;

                    //m_ModelMinimax.Minimax = 1;
                    //m_ModelMinimax.Opponent = -1;
                    //size_t action = m_ModelMinimax.playAction();

                    size_t action = 0;
                    float maxValue = -9999.9f;
                    for (size_t i = 0; i < 9; ++i)
                    {
                        if (m_Grid.isValid(i))
                        {
                            TicTacToeGrid temp = m_Grid;
                            temp.Grid[i] = m_Turn;

                            Elysium::Matrix value;
                            std::vector<float> s_prime;
                            temp.getStateOfGrid(s_prime);
                            getInvertedState(s_prime);
                            m_TicTacToeModel.predict(Elysium::Matrix(s_prime), value);

                            if (value[{0, 0}] > maxValue)
                            {
                                action = i;
                                maxValue = value[{0, 0}];
                            }
                        }
                    }

                    addAction(getPosition(action), action);
                    std::thread trainingThread(&TicTacToeScene::trainModel, this, s);
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