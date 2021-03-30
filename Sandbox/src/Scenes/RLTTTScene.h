#pragma once

#include <Elysium.h>

#include "Game/Tic-Tac-Toe.h"

class TTTAgent : public Elysium::TabularRLAgent
{
public:
    std::unordered_map<std::string, unsigned int> StateVisitCount;
    bool Greedy = false;
    uint32_t AgentScore = 0;
    uint32_t AdversaryScore = 0;

    Elysium::State_Action_Pair ChosenPair;

public:
    TTTAgent(float learningRate, float discountFactor) : TabularRLAgent(learningRate, discountFactor, 0.0f)
    {
    }

    void updateValue(const std::string& currenStateCode, const Elysium::State& nextState)
    {
        this->updateStateValue(Elysium::State(currenStateCode), nextState);
    }

    void getVisitMapFromFile()
    {
        std::ifstream StateFile("res/AI/TTTStateVisits.rl");
        if (StateFile.is_open())
        {
            std::string line;
            std::string state;
            uint8_t counter = 0;

            while (getline(StateFile, line))
            {
                switch (counter)
                {
                case 0:
                    state = line;
                    counter++;
                    break;
                case 1:
                    StateVisitCount[state] = std::stoul(line);
                    counter = 0;
                    break;
                }
            }
            StateFile.close();
        }
    }

    void saveVisitMapToFiles()
    {
        if (!StateVisitCount.empty())
        {
            std::ofstream StateFile("res/AI/TTTStateVisits.rl");
            for (auto iterator : StateVisitCount)
            {
                StateFile << iterator.first << std::endl;
                StateFile << iterator.second << std::endl;
            }
            StateFile.close();
        }
    }
};

class RLTTTScene : public Elysium::Scene
{
private:
    float m_Height = 30.0f;

    Elysium::OrthographicCamera m_Camera;
    Elysium::Texture m_SpriteSheet;

    std::array<Elysium::Vector2, 9> m_Coins;
    std::array<uint32_t, 9> m_CoinsTextureIndexes = { 0 };
    Elysium::BodyHandle m_CoinIndex = 0;
    std::array<Elysium::TextureData, 2> m_CoinTextures; // 1 Blue, 2 Red

    TTTGrid m_Grid;
    uint32_t m_Turn = 1;
    float m_MoveCooldown = 1.0f;
    bool m_GameOver = false;
    bool m_Tie = false;
    uint32_t m_BlueScore = 0;
    uint32_t m_RedScore = 0;
    uint32_t m_DrawCount = 0;

    TTTAgent m_Agent;
    MinimaxPlayer m_Minimax;
    bool m_Random = false;
    bool m_SelfPlay = false;
    bool m_TrainWithMinimax = false;
    bool m_PlayAgainstMinimax = true;

private:
    void getPosition(Elysium::Action action, Elysium::Vector2& position);
    void printStateData(const std::string& stateCode);

    void addAction(Elysium::Vector2 position, size_t index);
    void updateAgent(float reward, bool terminal, const std::string& currentStateCode);

    void playAgainstMinimax();

    bool isWithinBounds(Elysium::Vector2 position, float x1, float y1, float x2, float y2);

public:
    RLTTTScene(unsigned int width, unsigned int height);
    ~RLTTTScene();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onMousePressedEvent(Elysium::MouseButtonPressedEvent& event);
    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};