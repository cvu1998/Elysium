#include "RLTTTScene.h"

#include "Game/Systems.h"

RLTTTScene::RLTTTScene(unsigned int width, unsigned int height) : 
    Elysium::Scene("Tic-Tac-Toe"),
    m_Camera(-m_Height * (float)(width / height), m_Height * (float)(width / height), -m_Height * 0.5f, m_Height * 0.5f),
    m_SpriteSheet("res/texture/platformPack_tilesheet.png"),
    m_Minimax(&m_Grid)
{
    e_PhysicsSystem2D.clear();
    m_CoinTextures[0] = m_SpriteSheet.getTextureData();
    m_CoinTextures[0].subtextureCoordinates({ 10, 6 }, { 128, 128 });

    m_CoinTextures[1] = m_SpriteSheet.getTextureData();
    m_CoinTextures[1].subtextureCoordinates({ 11, 5 }, { 128, 128 });

    m_Camera.setPosition({ 0.0f, 0.0f, 0.0f });
}

RLTTTScene::~RLTTTScene()
{
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

void RLTTTScene::addAction(Elysium::Vector2 position, size_t index)
{
    if (m_Grid.isValid(index))
    {
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
            m_Turn = Elysium::Random::Integer(1, 2);
        }
        m_MoveCooldown = -1.0f;
        m_Coins.fill({ 0.0f, 0.0f });
        m_CoinsTextureIndexes.fill(0);

        m_CoinIndex = 0;
        m_GameOver = false;

        m_Grid.clear();
    }

    if (m_PlayAgainstMinimax && m_MoveCooldown >= 0.0f)
    {
        if (m_Turn == 2)
        {
            m_Minimax.Minimax = 2;
            m_Minimax.Opponent = 1;
            size_t action = m_Minimax.playAction();

            Elysium::Vector2 position;
            getPosition((Elysium::Action)action, position);
            addAction(position, action);
        }
    }

    if (m_MoveCooldown < 0.0f)
        m_MoveCooldown += ts;

    e_PhysicsSystem2D.onUpdate(ts);

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
    ImGui::End();

    Elysium::Renderer::resetStats();
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
            addAction(position, index);;
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