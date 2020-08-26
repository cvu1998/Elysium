#pragma once

#include <Elysium.h>

#include "Game/Connect4Grid.h"

class Connect4Agent : public Elysium::RLAgent
{
public:
    float ExplorationProb = 0.1f;
    uint32_t AgentScore = 0;
    uint32_t AdversaryScore = 0;

    std::vector<Elysium::Action> Actions;
    Elysium::State_Action_Pair ChosenPair;

public:
    Connect4Agent(float learningRate, float discountFactor) : RLAgent(learningRate, discountFactor, 0.5f)
    {
        Actions.reserve(7);
        for (Elysium::Action a = 0; a < 7; a++)
            Actions.emplace_back(a);
    }

    void updateValue(const Elysium::State& nextState)
    {
        this->updateActionValueQL(ChosenPair, nextState, Actions);
    }
};

class RLConnect4Scene : public Elysium::Scene
{
private:
    float m_Height = 30.0f;

    Elysium::OrthographicCamera m_Camera;
    Texture m_SpriteSheet;

    std::array<Elysium::BodyHandle, 9> m_Rectangles;
    std::array<std::pair<Elysium::PhysicalBody*, uint32_t>, 42> m_Coins = { std::make_pair(nullptr, 0) };
    Elysium::BodyHandle m_CoinIndex = 0;
    std::array<TextureData, 2> m_CoinTextures; // 1 Blue, 2 Red

    Connect4Agent m_Agent;
    Connect4Agent m_TrainingAgent;

    Connect4Grid m_Grid;
    uint32_t m_Turn = 1;
    float m_MoveCooldown = 1.0f;
    bool m_GameOver = false;
    uint32_t m_BlueScore = 0;
    uint32_t m_RedScore = 0;

private:
    void addAction(float xposition, size_t column);
    void updateAgent(float gameOverReward, float defaultReward);

public:
    RLConnect4Scene(unsigned int width, unsigned int height);
    ~RLConnect4Scene();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onMousePressedEvent(Elysium::MouseButtonPressedEvent& event);
    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};