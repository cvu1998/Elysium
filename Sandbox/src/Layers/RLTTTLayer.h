#pragma once

#include <Elysium.h>

#include "Game/Tic-Tac-Toe.h"

class TTTAgent : public Elysium::RLAgent
{
public:
    float ExplorationProb = 0.1f;
    bool Greedy = false;
    uint32_t AgentScore = 0;
    uint32_t AdversaryScore = 0;

    std::vector<Elysium::Action> Actions;
    Elysium::State_Action_Pair ChosenPair;

public:
    TTTAgent(float learningRate, float discountFactor) : RLAgent(learningRate, discountFactor, 0.5f)
    {
        Actions.reserve(9);
        for (Elysium::Action a = 0; a < 9; a++)
            Actions.emplace_back(a);
    }

    void updateValue(const Elysium::State& nextState)
    {
        this->updateActionValueQL(ChosenPair, nextState, Actions);
    }
};

class RLTTTLayer : public Elysium::Layer
{
private:
    bool* m_RunConnect4;

    float m_Height = 30.0f;

    Elysium::OrthographicCamera m_Camera;
    Texture m_SpriteSheet;

    std::array<Elysium::Vector2, 9> m_Coins;
    std::array<uint32_t, 9> m_CoinsTextureIndexes = { 0 };
    Elysium::BodyHandle m_CoinIndex = 0;
    std::array<TextureData, 2> m_CoinTextures; // 1 Blue, 2 Red

    TTTAgent m_Agent;
    bool m_Random = false;

    TTTGrid m_Grid;
    uint32_t m_Turn = 1;
    float m_MoveCooldown = 1.0f;
    bool m_GameOver = false;
    uint32_t m_BlueScore = 0;
    uint32_t m_RedScore = 0;

private:
    void addAction(Elysium::Vector2 position, size_t index);
    void updateAgent(float gameOverReward, float defaultReward);

    bool isWithinBounds(Elysium::Vector2 position, float x1, float y1, float x2, float y2);

public:
    RLTTTLayer(bool* runConnect4, unsigned int width, unsigned int height);
    ~RLTTTLayer();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onKeyPressedEvent(Elysium::KeyPressedEvent& event);
    bool onMousePressedEvent(Elysium::MouseButtonPressedEvent& event);
    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};