#pragma once

#include <Elysium.h>

#include "Game/TicTacToeMinimax.h"

class TicTacToeScene : public Elysium::Scene
{
private:
    float m_Height = 30.0f;

    Elysium::OrthographicCamera m_Camera;
    Elysium::Texture m_SpriteSheet;

    std::array<Elysium::Vector2, 9> m_Coins;
    std::array<uint32_t, 9> m_CoinsTextureIndexes = { 0 };
    Elysium::BodyHandle m_CoinIndex = 0;
    std::array<Elysium::TextureData, 2> m_CoinTextures; // 1 Blue, 2 Red

    TicTacToeGrid m_Grid;
    int m_Turn = 1;
    bool m_GameOver = false;
    bool m_Tie = false;
    uint32_t m_BlueScore = 0;
    uint32_t m_RedScore = 0;
    uint32_t m_DrawCount = 0;
    bool m_RedWon = false;

    bool m_Pause = false;
    float m_MoveCooldown = 0.0f;
    static constexpr float s_Cooldown = -0.01f;

    TicTacToeMinimax m_Minimax;
    bool m_PlayAgainstMinimax = false;

    Elysium::Model m_TicTacToeModel;
    Elysium::Matrix m_Dataset;
    Elysium::Matrix m_EpisodeData;
    bool m_TrainModel = false;
    bool m_DoneTraining = true;
    bool m_PlayAgainstModel = true;
    TicTacToeMinimax m_ModelMinimax;

private:
    Elysium::Vector2 getPosition(Elysium::Action action);

    void addAction(Elysium::Vector2 position, size_t index);

    bool isWithinBounds(Elysium::Vector2 position, float x1, float y1, float x2, float y2);

    void loadDataset(const char* filepath);
    void saveDataset(const char* filepath);

    float evaluateState(const TicTacToeGrid& grid);
    void trainModel(const TicTacToeGrid& previous);

    void getInvertedState(std::vector<float>& state);

public:
    TicTacToeScene(unsigned int width, unsigned int height);
    ~TicTacToeScene();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onMousePressedEvent(Elysium::MouseButtonPressedEvent& event);
    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};