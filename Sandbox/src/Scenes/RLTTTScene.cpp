#include "RLTTTScene.h"

#include "Game/Systems.h"

RLTTTScene::RLTTTScene(unsigned int width, unsigned int height) : Scene("Tic-Tac-Toe"),
m_Camera(-m_Height * (float)(width / height), m_Height * (float)(width / height), -m_Height * 0.5f, m_Height * 0.5f),
m_SpriteSheet("res/texture/platformPack_tilesheet.png"),
m_Agent(0.1f, 0.8f),
m_Minimax(&m_Grid)
{
    e_PhysicsSystem.getBodies().clear();
    m_CoinTextures[0] = m_SpriteSheet.getTextureData();
    m_CoinTextures[0].subtextureCoordinates({ 10, 6 }, { 128, 128 });

    m_CoinTextures[1] = m_SpriteSheet.getTextureData();
    m_CoinTextures[1].subtextureCoordinates({ 11, 5 }, { 128, 128 });

    m_Camera.setPosition({ 0.0f, 0.0f, 0.0f });

    m_Agent.readFunctionFromFile("res/AI/TTTStateValueFunction.rl");
    m_Agent.getVisitMapFromFile();
}

RLTTTScene::~RLTTTScene()
{
    m_Agent.saveFunctionToFile("res/AI/TTTStateValueFunction.rl");
    m_Agent.saveVisitMapToFiles();
}

void RLTTTScene::getPosition(Elysium::Action action, Elysium::Vector2& position)
{
    switch (action)
    {
    case 0:
        position = { -6.0f, -6.0f };
        break;
    case 1:
        position = { 0.0f, -6.0f };
        break;
    case 2:
        position = { 6.0f, -6.0f };
        break;
    case 3:
        position = { -6.0f, 0.0f };
        break;
    case 4:
        position = { 0.0f, 0.0f };
        break;
    case 5:
        position = { 6.0f, 0.0f };
        break;
    case 6:
        position = { -6.0f, 6.0f };
        break;
    case 7:
        position = { 0.0f, 6.0f };
        break;
    case 8:
        position = { 6.0f, 6.0f };
        break;
    }
}

void RLTTTScene::printStateData(const std::string& stateCode)
{
#ifdef _DEBUG
    m_Grid.printGrid(stateCode);
    ELY_INFO("Value of state: {0}", m_Agent.StateValueFunction[stateCode]);
#endif
}

void RLTTTScene::addAction(Elysium::Vector2 position, size_t index)
{
    if (m_Grid.isValid(index))
    {
        if (m_Random || m_SelfPlay || m_TrainWithMinimax)
            m_MoveCooldown = -0.01f;
        else
            m_MoveCooldown = -0.5f;

        m_Coins[m_CoinIndex] = position;
        m_CoinsTextureIndexes[m_CoinIndex++] = m_Turn;
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
            case 1:
                m_BlueScore++;
                break;
            case 2:
                m_RedScore++;
                break;
            }
        }

        switch (m_Turn)
        {
        case 1:
            m_Turn = 2;
            break;
        case 2:
            m_Turn = 1;
            break;
        }
    }
}

void RLTTTScene::updateAgent(float reward, bool terminal, const std::string& currentStateCode)
{
    std::string nextStateCode;
    if (!terminal)
        m_Grid.getCurrentStateCode(nextStateCode);
    Elysium::State nextState = Elysium::State(nextStateCode, reward, terminal);
    m_Agent.updateValue(currentStateCode, nextState);
}

void RLTTTScene::playAgainstMinimax()
{
    if (m_MoveCooldown >= 0.0f)
    {
        if (m_PlayAgainstMinimax)
        {
            if (m_Turn == 2 && !(m_TrainWithMinimax || m_Random))
            {
                m_Minimax.Minimax = 2;
                m_Minimax.Opponent = 1;
                size_t action = m_Minimax.playAction();

                Elysium::Vector2 position;
                getPosition((Elysium::Action)action, position);
                addAction(position, action);
            }
        }
    }
}

bool RLTTTScene::isWithinBounds(Elysium::Vector2 position, float x1, float y1, float x2, float y2)
{
    bool CollisionX = position.x >= x1 && position.x <= x2;
    bool CollisionY = position.y >= y1 && position.y <= y2;
    return CollisionX && CollisionY;
}

void RLTTTScene::onUpdate(Elysium::Timestep ts)
{
    if (m_GameOver && m_MoveCooldown >= 0.0f)
    {
        if (m_Tie)
        {
            m_DrawCount++;
            m_Turn = Random::Integer(1, 2);
        }

        if (!m_PlayAgainstMinimax)
        {
            std::string currentStateCode;
            m_Grid.getCurrentStateCode(currentStateCode);
            printStateData(currentStateCode);

            float reward = 0.0f;
            if (m_Agent.AdversaryScore < m_BlueScore)
                reward = -1.0f;
            else if (m_Agent.AgentScore < m_RedScore)
                reward = 1.0f;

            m_Agent.AgentScore = m_RedScore;
            m_Agent.AdversaryScore = m_BlueScore;
            updateAgent(reward, true, currentStateCode);
            printStateData(currentStateCode);
        }

        m_MoveCooldown = -1.0f;
        m_Coins.fill( {0.0f, 0.0f} );
        m_CoinsTextureIndexes.fill(0);

        m_CoinIndex = 0;
        m_GameOver = false;

        m_Grid.clear();

        m_Agent.saveFunctionToFile("res/AI/TTTStateValueFunction.rl");
        m_Agent.saveVisitMapToFiles();
    }

    playAgainstMinimax();

    if (!m_PlayAgainstMinimax && m_MoveCooldown >= 0.0f)
    {
        if (m_Turn == 1)
        {
            if (m_TrainWithMinimax || m_SelfPlay || m_Random)
            {
                std::string currentStateCode;
                m_Grid.getCurrentStateCode(currentStateCode);

                if (m_TrainWithMinimax && !m_SelfPlay && !m_Random)
                {
                    m_Minimax.Minimax = 2;
                    m_Minimax.Opponent = 1;
                    size_t action = m_Minimax.playAction();

                    Elysium::Vector2 position;
                    getPosition((Elysium::Action)action, position);
                    addAction(position, action);
                }
                else if (!m_Random && m_SelfPlay && !m_TrainWithMinimax)
                {
                    m_Agent.StateVisitCount[currentStateCode]++;
                    float explorationProb = 25.0f / ((25.0f + (float)m_Agent.StateVisitCount[currentStateCode]));
                    float probability = Random::Float();

                    ELY_INFO("MinAgent probabilities: {0}, {1}", explorationProb, probability);
                    if (probability < explorationProb && !m_Agent.Greedy)
                    {
                        Elysium::Action action = Random::Integer(0, 8);
                        while (!m_Grid.isValid(action))
                            action = Random::Integer(0, 8);
                        m_Agent.ChosenPair = { Elysium::State(currentStateCode), action };
                    }
                    else
                    {
                        std::vector<Elysium::Action> randomAction;
                        float minValue = std::numeric_limits<float>::max();
                        for (Elysium::Action action = 0; action < 9; action++)
                        {
                            if (m_Grid.isValid(action))
                            {
                                std::string nextStateCode;
                                m_Grid.getNextStateCode(nextStateCode, action, m_Turn);
                                printStateData(nextStateCode);

                                float value = m_Agent.getStateValue(Elysium::State(nextStateCode));
                                if (value < minValue)
                                {
                                    minValue = value;
                                    m_Agent.ChosenPair.Action = action;
                                    randomAction.clear();
                                }
                                else if (minValue == value)
                                {
                                    randomAction.push_back(action);
                                }
                            }
                        }

                        if (!randomAction.empty())
                        {
                            m_Agent.ChosenPair.Action = randomAction[Random::Integer(0, (int)randomAction.size() - 1)];
                        }
                        m_Agent.ChosenPair.State = Elysium::State(currentStateCode);
                        ELY_INFO("MinAgent chose action and value: {0}, {1}", minValue, m_Agent.ChosenPair.Action);
                    }
                    Elysium::Vector2 position;
                    getPosition(m_Agent.ChosenPair.Action, position);
                    addAction(position, m_Agent.ChosenPair.Action);
                }
                else if (m_Random && !m_SelfPlay && !m_TrainWithMinimax)
                {
                    Elysium::Action action = Random::Integer(0, 8);
                    while (!m_Grid.isValid(action))
                        action = Random::Integer(0, 8);

                    Elysium::Vector2 position;
                    getPosition(action, position);
                    addAction(position, action);
                }

                updateAgent(0.0f, false, currentStateCode);
            }
        }
        else if (m_Turn == 2)
        {
            std::string currentStateCode;
            m_Grid.getCurrentStateCode(currentStateCode);
            printStateData(currentStateCode);

            m_Agent.StateVisitCount[currentStateCode]++;
            float explorationProb = 25.0f / ((25.0f + (float)m_Agent.StateVisitCount[currentStateCode]));
            float probability = Random::Float();

            ELY_INFO("MaxAgent probabilities: {0}, {1}", explorationProb, probability);
            if (probability < explorationProb && !m_Agent.Greedy)
            {
                Elysium::Action action = Random::Integer(0, 8);
                while (!m_Grid.isValid(action))
                    action = Random::Integer(0, 8);
                m_Agent.ChosenPair = { Elysium::State(currentStateCode), action };
            }
            else
            {
                std::vector<Elysium::Action> randomAction;
                float maxValue = -std::numeric_limits<float>::max();
                for (Elysium::Action action = 0; action < 9; action++)
                {
                    if (m_Grid.isValid(action))
                    {
                        std::string nextStateCode;
                        m_Grid.getNextStateCode(nextStateCode, action, m_Turn);
                        printStateData(nextStateCode);

                        float value = m_Agent.getStateValue(Elysium::State(nextStateCode));
                        if (maxValue < value)
                        {
                            maxValue = value;
                            m_Agent.ChosenPair.Action = action;
                            randomAction.clear();
                        }
                        else if (maxValue == value)
                        {
                            randomAction.push_back(action);
                        }
                    }
                }

                if (!randomAction.empty())
                {
                    m_Agent.ChosenPair.Action = randomAction[Random::Integer(0, (int)randomAction.size() - 1)];
                }
                m_Agent.ChosenPair.State = Elysium::State(currentStateCode);
                ELY_INFO("MaxAgent chose action and value: {0}, {1}", maxValue, m_Agent.ChosenPair.Action);
            }
            Elysium::Vector2 position;
            getPosition(m_Agent.ChosenPair.Action, position);
            addAction(position, m_Agent.ChosenPair.Action);

            updateAgent(0.0f, false, currentStateCode);
        }
    }

    ImGui::Begin("Agent's Options");
    ImGui::Checkbox("Train With Minimax", &m_TrainWithMinimax);
    ImGui::Checkbox("Train Using Self Play", &m_SelfPlay);
    ImGui::Checkbox("Train With Random", &m_Random);
    ImGui::Checkbox("Play Against Minimax", &m_PlayAgainstMinimax);
    ImGui::Checkbox("Greedy", &m_Agent.Greedy);
    ImGui::Text("Agent Learning Rate: %f", m_Agent.LearningRate);
    ImGui::End();

    if (m_MoveCooldown < 0.0f)
        m_MoveCooldown += ts;

    e_PhysicsSystem.onUpdate(ts);

    Elysium::Renderer2D::beginScene(m_Camera);
    float position = 3.0f;
    float size = 18.0f;
    float pos = 6.0f;
    Elysium::Renderer2D::drawQuad({ 0.0f, position }, { size, 2.0f }, { 0.0f, 0.0f, 1.0f, 1.0f });
    Elysium::Renderer2D::drawQuad({ 0.0f, -position }, { size, 2.0f }, { 0.0f, 0.0f, 1.0f, 1.0f });
    Elysium::Renderer2D::drawQuad({ position, 0.0f }, { 2.0f, size }, { 0.0f, 0.0f, 1.0f, 1.0f });
    Elysium::Renderer2D::drawQuad({ -position, 0.0f }, { 2.0f, size }, { 0.0f, 0.0f, 1.0f, 1.0f });

    for (size_t i = 0; i < m_CoinIndex; i++)
    {
        Elysium::Renderer2D::drawQuad(m_Coins[i], { 2.0f, 2.0f }, m_CoinTextures[m_CoinsTextureIndexes[i] - 1]);
    }
    Elysium::Renderer2D::endScene();

    ImGui::Begin("Tic-Tac-Toe");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Number of Draw Calls: %d", Elysium::Renderer2D::getStats().DrawCount);
    ImGui::Text("Number of Quads: %d", Elysium::Renderer2D::getStats().QuadCount);
    ImGui::Text("Red: %d : Blue %d, Draws: %d", m_RedScore, m_BlueScore, m_DrawCount);
    ImGui::End();

    Elysium::Renderer2D::resetStats();
}

void RLTTTScene::onEvent(Elysium::Event& event)
{
    Elysium::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Elysium::MouseButtonPressedEvent>(BIND_EVENT_FUNCTION(RLTTTScene::onMousePressedEvent));
    dispatcher.Dispatch<Elysium::WindowResizeEvent>(BIND_EVENT_FUNCTION(RLTTTScene::onWindowResizeEvent));
}

bool RLTTTScene::onMousePressedEvent(Elysium::MouseButtonPressedEvent& event)
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
        {
            std::string currentStateCode;
            if (!m_PlayAgainstMinimax)
            {
                m_Grid.getCurrentStateCode(currentStateCode);
                printStateData(currentStateCode);
            }

            addAction(position, index);;

            if (!m_PlayAgainstMinimax)
                updateAgent(0.0f, false, currentStateCode);
        }
    }
    return false;
}

bool RLTTTScene::onWindowResizeEvent(Elysium::WindowResizeEvent& event)
{
    unsigned int width = event.getWidth();
    unsigned int height = event.getHeight();
    if (width > 0 && height > 0)
        m_Camera.setProjection(-m_Height * (float)(width / height), m_Height * (float)(width / height),
            -m_Height * 0.5f, m_Height * 0.5f);
    return false;
}