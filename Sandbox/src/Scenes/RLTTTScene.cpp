#include "RLTTTScene.h"

#include "Game/Systems.h"

RLTTTScene::RLTTTScene(unsigned int width, unsigned int height) : Scene("Tic-Tac-Toe"),
m_Camera(-m_Height * (float)(width / height), m_Height * (float)(width / height), -m_Height * 0.5f, m_Height * 0.5f),
m_SpriteSheet("res/texture/platformPack_tilesheet.png"),
m_Agent(0.8f, 0.8f),
m_TrainingAgent(0.0f, 0.0f)
{
    e_PhysicsSystem.getBodies().clear();
    m_CoinTextures[0] = m_SpriteSheet.getTextureData();
    m_CoinTextures[0].subtextureCoordinates({ 10, 6 }, { 128, 128 });

    m_CoinTextures[1] = m_SpriteSheet.getTextureData();
    m_CoinTextures[1].subtextureCoordinates({ 11, 5 }, { 128, 128 });

    m_Camera.setPosition({ 0.0f, 0.0f, 0.0f });

    m_Agent.readPolicyFromFile("res/AI/TTTPolicy.rl");
    m_TrainingAgent.readPolicyFromFile("res/AI/TTTPolicy.rl");
}

RLTTTScene::~RLTTTScene()
{
    m_Agent.savePolicyToFile("res/AI/TTTPolicy.rl");
}

void RLTTTScene::addAction(Elysium::Vector2 position, size_t index)
{
    if (m_Grid.isValid(index))
    {
        m_MoveCooldown = -0.1f;

        m_Coins[m_CoinIndex] = position;
        m_CoinsTextureIndexes[m_CoinIndex++] = m_Turn;
        m_Grid.Grid[index] = m_Turn;

        if (m_CoinIndex >= 9)
            m_GameOver = true;

        if (m_Grid.isWinningMove(index, m_Turn))
        {
            m_GameOver = true;
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
        m_Grid.printGrid();
    }
}

void RLTTTScene::updateAgent(float gameOverReward, float defaultReward)
{
    float reward = m_GameOver ? gameOverReward : defaultReward;
    if (m_CoinIndex >= 9 && m_Agent.AgentScore == m_RedScore && m_Agent.AdversaryScore == m_BlueScore)
        reward = 0.0f;

    m_Agent.AgentScore = m_RedScore;
    m_Agent.AdversaryScore = m_BlueScore;
    std::string nextStateCode;
    m_Grid.getCurrentStateCode(nextStateCode);
    Elysium::State nextState = Elysium::State(nextStateCode, reward, m_GameOver);
    m_Agent.updateValue(nextState);
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
        m_MoveCooldown = -1.0f;
        m_Coins.fill( {0.0f, 0.0f} );
        m_CoinsTextureIndexes.fill(0);

        m_CoinIndex = 0;
        m_GameOver = false;

        m_Grid.clear();

        m_Agent.savePolicyToFile("res/AI/TTTPolicy.rl");
    }

    if (m_Turn == 1 && m_MoveCooldown >= 0.0f && !m_Random && m_Training)
    {
        std::string currentStateCode;
        m_Grid.getCurrentStateCode(currentStateCode);

        float maxValue = -std::numeric_limits<float>::max();
        Elysium::Action chosenAction;
        std::vector<Elysium::Action> randomAction;
        for (Elysium::Action action = 0; action < 9; action++)
        {
            if (m_Grid.isValid(action))
            {
                float value = m_TrainingAgent.getStateActionPairValue({ Elysium::State(currentStateCode), action });

                if (maxValue < value)
                {
                    maxValue = value;
                    chosenAction = action;
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
            chosenAction = randomAction[Random::Integer(0, (int)randomAction.size() - 1)];
        }
        m_TrainingAgent.ChosenPair = { Elysium::State(currentStateCode), chosenAction };

        Elysium::Vector2 position = { 0.0f, 0.0f };
        switch (m_TrainingAgent.ChosenPair.Action)
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
        addAction(position, m_TrainingAgent.ChosenPair.Action);

        if (m_GameOver)
            updateAgent(-1.0f, 0.0f);
    }

    if (m_Turn == 1 && m_MoveCooldown >= 0.0f && m_Random && !m_Training)
    {
        Elysium::Action action = Random::Integer(0, 8);
        while (!m_Grid.isValid(action))
            action = Random::Integer(0, 8);

        Elysium::Vector2 position = { 0.0f, 0.0f };
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
        addAction(position, action);

        if (m_GameOver)
            updateAgent(-1.0f, 0.0f);
    }

    if (m_Turn == 2 && m_MoveCooldown >= 0.0f)
    {
        std::string currentStateCode;
        m_Grid.getCurrentStateCode(currentStateCode);

        float probability = Random::Float();
        if (probability < m_Agent.ExplorationProb)
        {
            Elysium::Action action = Random::Integer(0, 8);
            while (!m_Grid.isValid(action))
                action = Random::Integer(0, 8);
            m_Agent.ChosenPair = { Elysium::State(currentStateCode), action };
        }
        else
        {
            float maxValue = -std::numeric_limits<float>::max();
            Elysium::Action chosenAction;
            std::vector<Elysium::Action> randomAction;
            for (Elysium::Action action = 0; action < 9; action++)
            {
                if (m_Grid.isValid(action))
                {
                    float value = m_Agent.getStateActionPairValue({ Elysium::State(currentStateCode), action });

                    if (maxValue < value)
                    {
                        maxValue = value;
                        chosenAction = action;
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
                chosenAction = randomAction[Random::Integer(0, (int)randomAction.size() - 1)];
            }
            m_Agent.ChosenPair = { Elysium::State(currentStateCode), chosenAction };
        }
        Elysium::Vector2 position = { 0.0f, 0.0f };
        switch (m_Agent.ChosenPair.Action)
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
        addAction(position, m_Agent.ChosenPair.Action);

        updateAgent(1.0f, 0.0f);
    }

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
    ImGui::Text("Red: %d : Blue %d", m_RedScore, m_BlueScore);
    ImGui::Checkbox("Training Agent", &m_Training);
    ImGui::Checkbox("Random Agent", &m_Random);
    ImGui::Checkbox("Greedy", &m_Agent.Greedy);
    ImGui::End();

    m_Agent.ExplorationProb = m_Agent.Greedy ? 0.0f : 0.1f;

    Elysium::Renderer2D::resetStats();
}

void RLTTTScene::onEvent(Elysium::Event& event)
{
    Elysium::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Elysium::KeyPressedEvent>(BIND_EVENT_FUNCTION(RLTTTScene::onKeyPressedEvent));
    dispatcher.Dispatch<Elysium::MouseButtonPressedEvent>(BIND_EVENT_FUNCTION(RLTTTScene::onMousePressedEvent));
    dispatcher.Dispatch<Elysium::WindowResizeEvent>(BIND_EVENT_FUNCTION(RLTTTScene::onWindowResizeEvent));
}

bool RLTTTScene::onKeyPressedEvent(Elysium::KeyPressedEvent& event)
{
    if (!m_GameOver && m_Turn == 1 && m_MoveCooldown >= 0.0f)
    {
        Elysium::Vector2 position = { 0.0f, 0.0f };
        size_t index = 0;
        if (Elysium::Input::isKeyPressed(ELY_KEY_1))
        {
            position = { -6.0f, -6.0f };
            index = 0;
        }
        else if (Elysium::Input::isKeyPressed(ELY_KEY_2))
        {
            position = { 0.0f, -6.0f };
            index = 1;
        }
        else if (Elysium::Input::isKeyPressed(ELY_KEY_3))
        {
            position = { 6.0f, -6.0f };
            index = 2;
        }
        else if (Elysium::Input::isKeyPressed(ELY_KEY_4))
        {
            position = { -6.0f, 0.0f };
            index = 3;
        }
        else if (Elysium::Input::isKeyPressed(ELY_KEY_5))
        {
            position = { 0.0f, 0.0f };
            index = 4;
        }
        else if (Elysium::Input::isKeyPressed(ELY_KEY_6))
        {
            position = { 6.0f, 0.0f };
            index = 5;
        }
        else if (Elysium::Input::isKeyPressed(ELY_KEY_7))
        {
            position = { -6.0f, 6.0f };
            index = 6;
        }
        else if (Elysium::Input::isKeyPressed(ELY_KEY_8))
        {
            position = { 0.0f, 6.0f };
            index = 7;
        }
        else if (Elysium::Input::isKeyPressed(ELY_KEY_9))
        {
            position = { 6.0f, 6.0f };
            index = 8;
        }
        addAction(position, index);

        if (m_GameOver)
            updateAgent(-1.0f, 0.0f);
    }
    return false;
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
            addAction(position, index);;

            if (m_GameOver)
                updateAgent(-1.0f, 0.0f);
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