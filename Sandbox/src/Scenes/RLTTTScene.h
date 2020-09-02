#pragma once

#include <Elysium.h>

#include "Game/Tic-Tac-Toe.h"

class TTTAgent : public Elysium::RLAgent
{
public:
    float ExplorationProb = 0.3f;
    bool Greedy = false;
    uint32_t AgentScore = 0;
    uint32_t AdversaryScore = 0;

    Elysium::State_Action_Pair ChosenPair;

public:
    TTTAgent(float learningRate, float discountFactor) : RLAgent(learningRate, discountFactor, 0.0f)
    {
    }

    void updateValue(const Elysium::State& nextState)
    {
        this->updateActionValueQL(ChosenPair, nextState, {0, 1, 2, 3, 4, 5, 6, 7, 8});
    }
};

class RLTTTScene : public Elysium::Scene
{
private:
    float m_Height = 30.0f;

    Elysium::OrthographicCamera m_Camera;
    Texture m_SpriteSheet;

    std::array<Elysium::Vector2, 9> m_Coins;
    std::array<uint32_t, 9> m_CoinsTextureIndexes = { 0 };
    Elysium::BodyHandle m_CoinIndex = 0;
    std::array<TextureData, 2> m_CoinTextures; // 1 Blue, 2 Red

    TTTGrid m_Grid;
    uint32_t m_Turn = 1;
    float m_MoveCooldown = 1.0f;
    bool m_GameOver = false;
    uint32_t m_BlueScore = 0;
    uint32_t m_RedScore = 0;

    TTTAgent m_Agent;
    MinimaxPlayer m_Minimax;
    bool m_Random = false;
    bool m_TrainWithMinimax = false;
    bool m_PlayAgainstMinimax = true;

private:
    void getPosition(Elysium::Action action, Elysium::Vector2& position);

    void addAction(Elysium::Vector2 position, size_t index);
    void updateAgent(float gameOverReward, float defaultReward);

    void minimaxPlay();

    bool isWithinBounds(Elysium::Vector2 position, float x1, float y1, float x2, float y2);

public:
    RLTTTScene(unsigned int width, unsigned int height);
    ~RLTTTScene();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onMousePressedEvent(Elysium::MouseButtonPressedEvent& event);
    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};