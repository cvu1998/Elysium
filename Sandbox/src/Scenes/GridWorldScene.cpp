#include "GridWorldScene.h"

GridWorldScene::GridWorldScene(unsigned int width, unsigned int height) :
    Elysium::Scene("Grid World"),
    m_Camera(-s_Height * (float)(width / height), s_Height* (float)(width / height), -s_Height * 0.5f, s_Height * 0.5f),
    m_FireSprites("res/texture/fire.png"),
    m_GoldSprites("res/texture/gold.png"),
    m_CatSprites("res/texture/cat.png"),
    m_GridModel(s_InputSize),
    m_TargetModel(s_InputSize),
    m_Dataset(0, s_DataLength, true),
    m_EpisodeData(0, s_DataLength, true)
{
    m_FireTexture = m_FireSprites.getTextureData();
    m_FireTexture.subtextureCoordinates({ 0, 5 }, { 64, 64 });

    m_GoldTexture = m_GoldSprites.getTextureData();
    m_GoldTexture.subtextureCoordinates({ 1, 0 }, { 32, 32 });

    m_CatTexture = m_CatSprites.getTextureData();
    m_CatTexture.subtextureCoordinates({ 0, 5 }, { 118, 150 });

    m_GridModel.add(new Elysium::Dense(s_HiddenSize, Elysium::AI::Activation::RELU, Elysium::AI::Initializer::HE));
    m_GridModel.add(new Elysium::Dense(s_OutputSize, Elysium::AI::Activation::LINEAR));
    m_GridModel.LossFunction = Elysium::AI::Loss::MEAN_SQUARED;
    m_GridModel.LearningRate = s_LearningRate;

    m_TargetModel.add(new Elysium::Dense(s_HiddenSize, Elysium::AI::Activation::RELU, Elysium::AI::Initializer::HE));
    m_TargetModel.add(new Elysium::Dense(s_OutputSize, Elysium::AI::Activation::LINEAR));

    if (m_GridModel.load("res/AI/Grid-World/grid-model"))
        loadDataset("res/AI/Grid-World/grid-dataset.csv");
    m_GridModel.summary();

    updateTarget();

    Elysium::Renderer::setLineWidth(2.0f);

    constexpr float x0 = -6.0f;
    constexpr float y0 = -6.0f;
    constexpr float size = 6.0f;
    constexpr float increment = 6.0f;
    for (size_t i = 0; i < m_Grid.size(); ++i)
    {
        float x = x0 + (increment * (float)(i % 3));
        float y = y0 + (increment * (float)(i / 3));
        m_GridCoordinates[i] = { x, y };
    }

    Elysium::Render::ClearColor({ 0.5f, 0.0f, 0.5f, 1.0f });
}

GridWorldScene::~GridWorldScene()
{
    Elysium::Renderer::setLineWidth(1.0f);

    Elysium::Render::ClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });

    m_GridModel.save("res/AI/Grid-World/grid-model");
    saveDataset("res/AI/Grid-World/grid-dataset.csv");
}

void GridWorldScene::onUpdate(Elysium::Timestep ts)
{
    if (!m_Pause)
    {
        if (m_DoneTraining)
        {
            if (m_GameOver)
            {
                m_MoveCooldown = s_Cooldown;

                m_Wins += m_CatPosition == s_Goal ? 1 : 0;
                m_HundoWins += m_CatPosition == s_Goal ? 1 : 0;

                m_CatPosition = s_Start;

                m_Games++;
                m_GameOver = false;
                m_Grid.fill(0);
                m_Grid[m_CatPosition] = 1.0f;

                m_GridModel.save("res/AI/grid-model");
                saveDataset("res/AI/grid-dataset.csv");

                if (m_Games % 100 == 0)
                {
                    m_WinRate = (float)m_HundoWins / 100.0f;
                    m_HundoWins = 0;
                }

                if (m_Games % 100 == 0 && m_Policy > 0)
                    printStats();
            }

            if (m_MoveCooldown >= 0.0f)
            {
                m_MoveCooldown = s_Cooldown;

                ELY_INFO("Cat Position: {0}", m_CatPosition);
                size_t action = chooseAction();

                std::vector<float> state(m_Grid.begin(), m_Grid.end());
                m_Grid[m_CatPosition] = 0.0f;
                m_CatPosition = m_CatPosition + m_Moves[action];
                m_Grid[m_CatPosition] = 1.0f;

                if (m_CatPosition == s_Fire1 || m_CatPosition == s_Fire2 || m_CatPosition == s_Goal)
                    m_GameOver = true;

                if (m_Policy > 0)
                {
                    if (++m_TargetUpdate % s_Update == 0)
                        updateTarget();

                    m_DoneTraining = false;
                    std::thread trainingThread(&GridWorldScene::trainModel, this, state, action);
                    trainingThread.detach();
                }
            }
            m_MoveCooldown += ts;
        }
    }

    Elysium::Renderer::beginScene(m_Camera);
    constexpr float x0 = -6.0f;
    constexpr float y0 = -6.0f;
    constexpr float size = 6.0f;
    constexpr float increment = 6.0f;
    for (size_t i = 0; i < m_Grid.size(); ++i)
    {
        float x = x0 + (increment * (float)(i % 3));
        float y = y0 + (increment * (float)(i / 3));
        switch (i)
        {
        case 5:
            Elysium::Renderer::drawQuad({ x, y }, { size, size }, m_FireTexture);
            break;
        case 6:
            Elysium::Renderer::drawQuad({ x, y }, { size, size }, m_FireTexture);
            break;
        case 8:
            Elysium::Renderer::drawQuad({ x, y }, { size, size }, m_GoldTexture);
            break;
        default:
            Elysium::Renderer::drawQuad({ x, y }, { size, size }, { 0.0f, 0.0f, 1.0f, 1.0f });
        }
    }
    Elysium::Renderer::drawQuad(m_GridCoordinates[m_CatPosition], { size, size }, m_CatTexture);
    Elysium::Renderer::endScene();

    Elysium::Renderer::beginScene(m_Camera);
    constexpr float x1 = -9.0f;
    constexpr float x2 =  9.0f;
    constexpr float y1 = -9.0f;
    constexpr float y2 =  9.0f;
    constexpr float inc = 6.0f;
    for (size_t i = 0; i < 4; ++i)
    {
        float v = y1 + (inc * (float)(i % 4));

        Elysium::Renderer::drawLine({ x1, v, 0.0f }, { x2, v, 0.0f }, { 0.0f, 0.0f, 0.0f, 1.0f });
        Elysium::Renderer::drawLine({ v, y1, 0.0f }, { v, y2, 0.0f }, { 0.0f, 0.0f, 0.0f, 1.0f });
    }
    Elysium::Renderer::endScene();

    ImGui::Begin("Grid World");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Games: %zu, Wins: %zu", m_Games, m_Wins);
    ImGui::Text("Win Rate: %f, Win Rate Per Hundred Games: %f", (float)m_Wins / (float)m_Games, m_WinRate);
    ImGui::Checkbox("Pause Game", &m_Pause);
    ImGui::End();
}

bool GridWorldScene::isValid(size_t action)
{
    int newPos = (int)m_CatPosition + m_Moves[action];
    bool inside = newPos < 9 && newPos >= 0;
    if (abs(m_Moves[action]) == 1)
        return inside && newPos / 3 == (int)m_CatPosition / 3;
    return inside;
}

void GridWorldScene::loadDataset(const char* filepath)
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
                sscanf_s(line.c_str(), " \
                    %f, %f, %f, %f, %f, %f, %f, %f, %f, \
                    %f, %f, %f, %f, %f, %f, %f, %f, %f, \
                    A: %f, R: %f, T: %d",
                    &row[0], &row[1], &row[2], &row[3], &row[4], &row[5], &row[6], &row[7], &row[8],
                    &row[9], &row[10], &row[11], &row[12], &row[13], &row[14], &row[15], &row[16], &row[17],
                    &action, &reward, &terminal);
                row[18] = action;
                row[19] = reward;
                m_TerminalStates.push_back(terminal);
                m_Dataset.appendRow(row);
            }
        }
        file.close();
    }
}

void GridWorldScene::saveDataset(const char* filepath)
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
                case 18:
                    file << "A: " << m_Dataset(i, j) << ", ";
                    break;
                case 19:
                    file << "R: " << m_Dataset(i, j) << ", T: " << m_TerminalStates[i] << std::endl;
                    break;
                default:
                    file << m_Dataset(i, j) << ", ";
                }
            }
        }
    }
    file.close();
}

size_t GridWorldScene::chooseAction()
{
    Elysium::Matrix qValues;
    std::vector<float> state(m_Grid.begin(), m_Grid.end());

    m_GridModel.predict(Elysium::Matrix(state), qValues);

    size_t action = 0;
    switch (m_Policy)
    {
    case 0:
        action = getMaxAction(qValues);
        break;
    case 1:
        action = getSoftmaxAction(qValues);
        break;
    }

    return action;
}

size_t GridWorldScene::getMaxAction(const Elysium::Matrix& QValues)
{
    float maxValue = -FLT_MAX;
    size_t action = 0;
    bool found = false;
    for (size_t i = 0; i < m_Moves.size(); ++i)
    {
        if (isValid(i))
        {
            if (!found)
            {
                action = i;
                found = true;
            }

            if (maxValue < QValues.Values[i])
            {
                maxValue = QValues.Values[i];
                action = i;
            }
        }
    }
    ELY_INFO("Action: {0}", action);

    return action;
}

size_t GridWorldScene::getSoftmaxAction(const Elysium::Matrix& QValues)
{
    std::vector<float> values, distribution;
    std::vector<size_t> actions;
    for (size_t i = 0; i < m_Moves.size(); ++i)
    {
        if (isValid(i))
        {
            values.push_back(QValues.Values[i]);
            actions.push_back(i);

            printf("%zd : %f, ", i, QValues.Values[i]);
        }
    }
    printf("\n");

    Elysium::AI::softmax(values, distribution, s_Beta);
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


        if (m_CatPosition == 7)
            printf("%zd : %f, ", actions[i], distribution[i]);
    }
    if (m_CatPosition == 7)
        printf("\n");

    ELY_INFO("Random: {0}, Action: {1}", random, action);

    return action;
}

void GridWorldScene::trainModel(const std::vector<float>& previous, size_t action)
{
    std::vector<float> state(m_Grid.begin(), m_Grid.end());
    float reward = 0.0f;
    if (m_GameOver)
    {
        reward = m_CatPosition == s_Fire1 || m_CatPosition == s_Fire2 ? -1.0f : 1.0f;
    }
    std::vector<float> data;
    switch (m_Algorithm)
    {
    case 0:
        updateDQNDataset(data, previous, state, action, reward);
        break;
    case 1:
        updateDQNDataset(data, previous, state, action, reward);
        break;
    case 2:
        updateMonteCarloDataset(data, previous, state, action, reward);
        break;
    }

    if (m_Dataset.getHeight() > s_BatchSize * 2)
    {
        constexpr size_t memorySize = 5000;
        if (m_Dataset.getHeight() > memorySize)
        {
            size_t index = m_Dataset.getHeight() - memorySize;
            m_Dataset = Elysium::Matrix::Slice(m_Dataset, index, 0, 0, 0);
            m_TerminalStates.erase(m_TerminalStates.begin(), m_TerminalStates.begin() + index);
        }

        switch (m_Algorithm)
        {
        case 0:
            DQN();
            break;
        case 1:
            modifiedDQN(data);
            break;
        case 2:
            MonteCarlo();
            break;
        }
    }
    m_DoneTraining = true;
}

void GridWorldScene::updateDQNDataset(std::vector<float>& data, const std::vector<float>& previous, const std::vector<float>& state, size_t action, float reward)
{
    data.insert(data.end(), previous.begin(), previous.end());
    data.insert(data.end(), m_Grid.begin(), m_Grid.end());
    data.push_back(float(action));
    data.push_back(reward);

    m_TerminalStates.push_back(m_GameOver ? 1 : 0);
    m_Dataset.appendRow(data);
}

void GridWorldScene::updateMonteCarloDataset(std::vector<float>& data, const std::vector<float>& previous, const std::vector<float>& state, size_t action, float reward)
{
    data.insert(data.end(), previous.begin(), previous.end());
    data.insert(data.end(), m_Grid.begin(), m_Grid.end());
    data.push_back(float(action));
    data.push_back(reward);

    m_TerminalStates.push_back(0);
    m_EpisodeData.appendRow(data);

    if (m_GameOver)
    {
        data.insert(data.end(), m_Grid.begin(), m_Grid.end());
        data.insert(data.end(), m_Grid.begin(), m_Grid.end());
        data.push_back(float(action));
        data.push_back(reward);

        m_TerminalStates.push_back(1);
        m_EpisodeData.appendRow(data);

        float discountedRewards = reward;
        for (size_t i = 0; i < m_EpisodeData.getHeight(); ++i)
        {
            size_t index = m_EpisodeData.getHeight() - 1;
            m_EpisodeData(index - i, 19) = discountedRewards;
            discountedRewards = s_DiscountFactor * discountedRewards;
        }
        m_Dataset = Elysium::Matrix::Concatenate(m_Dataset, m_EpisodeData);
        m_EpisodeData = Elysium::Matrix(0, s_DataLength);
    }
}

void GridWorldScene::DQN()
{
    size_t i = 0;
    Elysium::Random::InitInteger(0, (int)(m_Dataset.getHeight() - 1));
    Elysium::Matrix dataset(0, s_DataLength);
    std::vector<int> terminals(s_BatchSize);
    while (i < s_BatchSize)
    {
        size_t row = (size_t)Elysium::Random::Integer();
        dataset.appendRow(Elysium::Matrix::Slice(m_Dataset, row, row + 1, 0, 0).Values);
        terminals[i] = m_TerminalStates[row];
        i++;
    }

    Elysium::Matrix states = Elysium::Matrix::Slice(dataset, 0, 0, 0, 9);
    Elysium::Matrix nextStates = Elysium::Matrix::Slice(dataset, 0, 0, 9, 18);
    Elysium::Matrix actions = Elysium::Matrix::Slice(dataset, 0, 0, 18, 19);
    Elysium::Matrix rewards = Elysium::Matrix::Slice(dataset, 0, 0, 19, 20);

    Elysium::Matrix targets;
    m_GridModel.predict(states, targets);

    Elysium::Matrix nextQValues;
    m_TargetModel.predict(nextStates, nextQValues);

    std::vector<float> bestValues;
    bestValues.reserve(s_BatchSize);
    for (size_t i = 0; i < s_BatchSize; ++i)
    {
        float it = nextQValues(i, 0);
        for (size_t j = 1; j < nextQValues.getWidth(); ++j)
            it = glm::max(it, nextQValues(i, j));
        bestValues.emplace_back(it);
    }

    for (size_t i = 0; i < s_BatchSize; ++i)
    {
        targets(i, (size_t)actions.Values[i]) = terminals[i] ? rewards.Values[i] : rewards.Values[i] + s_DiscountFactor * bestValues[i];
    }

    m_GridModel.fit(states, targets, 1, s_BatchSize);
}

void GridWorldScene::modifiedDQN(const std::vector<float>& data)
{
    size_t targetSize = m_GameOver ? 1 : s_BatchSize;

    if (m_GameOver)
    {
        Elysium::Matrix dataset(data);

        Elysium::Matrix state = Elysium::Matrix::Slice(dataset, 0, 0, 0, 9);
        Elysium::Matrix action = Elysium::Matrix::Slice(dataset, 0, 0, 18, 19);
        Elysium::Matrix reward = Elysium::Matrix::Slice(dataset, 0, 0, 19, 20);

        Elysium::Matrix target;
        m_GridModel.predict(state, target);
        target(0, (size_t)action(0, 0)) = reward(0, 0);

        m_GridModel.LearningRate = s_ModifiedLearningRate;
        m_GridModel.fit(state, target, 1, targetSize);
        m_GridModel.LearningRate = s_LearningRate;
    }
    else
    {

        size_t i = 0;
        Elysium::Random::InitInteger(0, (int)(m_Dataset.getHeight() - 1));
        Elysium::Matrix dataset(0, s_DataLength);
        std::vector<int> terminals(s_BatchSize);
        while (i < s_BatchSize)
        {
            size_t row = (size_t)Elysium::Random::Integer();
            dataset.appendRow(Elysium::Matrix::Slice(m_Dataset, row, row + 1, 0, 0).Values);
            terminals[i] = m_TerminalStates[row];
            i++;
        }

        Elysium::Matrix states = Elysium::Matrix::Slice(dataset, 0, 0, 0, 9);
        Elysium::Matrix nextStates = Elysium::Matrix::Slice(dataset, 0, 0, 9, 18);
        Elysium::Matrix actions = Elysium::Matrix::Slice(dataset, 0, 0, 18, 19);
        Elysium::Matrix rewards = Elysium::Matrix::Slice(dataset, 0, 0, 19, 20);

        Elysium::Matrix targets;
        m_GridModel.predict(states, targets);

        Elysium::Matrix nextQValues;
        m_TargetModel.predict(nextStates, nextQValues);

        std::vector<float> bestValues;
        bestValues.reserve(s_BatchSize);
        for (size_t i = 0; i < s_BatchSize; ++i)
        {
            float it = nextQValues(i, 0);
            for (size_t j = 1; j < nextQValues.getWidth(); ++j)
                it = glm::max(it, nextQValues(i, j));
            bestValues.emplace_back(it);
        }

        for (size_t i = 0; i < s_BatchSize; ++i)
        {
            targets(i, (size_t)actions.Values[i]) = terminals[i] ? rewards.Values[i] : rewards.Values[i] + s_DiscountFactor * bestValues[i];
        }

        m_GridModel.fit(states, targets, 1, targetSize);
    }
}

void GridWorldScene::MonteCarlo()
{
    size_t i = 0;
    Elysium::Random::InitInteger(0, (int)(m_Dataset.getHeight() - 1));
    Elysium::Matrix dataset(0, s_DataLength);
    std::vector<int> terminals(s_BatchSize);
    while (i < s_BatchSize)
    {
        size_t row = (size_t)Elysium::Random::Integer();
        dataset.appendRow(Elysium::Matrix::Slice(m_Dataset, row, row + 1, 0, 0).Values);
        terminals[i] = m_TerminalStates[row];
        i++;
    }

    Elysium::Matrix states = Elysium::Matrix::Slice(dataset, 0, 0, 0, 9);
    Elysium::Matrix actions = Elysium::Matrix::Slice(dataset, 0, 0, 18, 19);
    Elysium::Matrix rewards = Elysium::Matrix::Slice(dataset, 0, 0, 19, 20);

    Elysium::Matrix targets;
    m_GridModel.predict(states, targets);

    for (size_t i = 0; i < s_BatchSize; ++i)
    {
        targets(i, (size_t)actions.Values[i]) = rewards.Values[i];
    }

    m_GridModel.fit(states, targets, 1, s_BatchSize);
}

void GridWorldScene::updateTarget()
{
    auto model = m_GridModel.getLayers();
    auto target = m_TargetModel.getLayers();
    for (size_t i = 0; i < model.size(); ++i)
    {
        target[i]->Weights.Values = model[i]->Weights.Values;
        target[i]->Biases = model[i]->Biases;
    }
}

void GridWorldScene::printStats()
{
    std::string filename;
    switch (m_Algorithm)
    {
    case 0:
        filename = "res/AI/DQN.txt";
        break;
    case 1:
        filename = "res/AI/Modified-DQN.txt";
        break;
    case 2:
        filename = "res/AI/Monte-Carlo.txt";
        break;
    }


    std::ofstream file(filename, std::ios_base::app);
    if (file.is_open())
    {
        file << "Games: " << m_Games << ", Wins: " << m_Wins << std::endl;
        file << "Win Rate: " << (float)m_Wins / (float)m_Games << ", Win Rate Per Hundred Games: " << m_WinRate << std::endl;
    }
    file.close();
}