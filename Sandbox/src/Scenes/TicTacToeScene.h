#pragma once

#include <Elysium.h>

#include "Game/TicTacToeMinimax.h"

class TicTacToeScene : public Elysium::Scene
{
private:
    float m_Height = 30.0f;

    Elysium::OrthographicCamera m_Camera;
    Elysium::Texture m_X;
    Elysium::Texture m_O;

    std::array<Elysium::Vector2, 9> m_Coins;
    std::array<uint32_t, 9> m_CoinsTextureIndexes = { 0 };
    Elysium::BodyHandle m_CoinIndex = 0;
    std::array<Elysium::TextureData, 2> m_CoinTextures; // 1 X, 2 O

    TicTacToeGrid m_Grid;
    int m_Turn = 1;
    bool m_GameOver = false;
    bool m_Tie = false;
    uint32_t m_XScore = 0;
    uint32_t m_OScore = 0;
    uint32_t m_DrawCount = 0;

    int m_GamesPlayed = 0;
    int m_Wins = 0;
    int m_Draws = 0;
    float m_WinRate = 0.0f;
    float m_DrawRate = 0.0f;

    bool m_Pause = false;
    float m_MoveCooldown = 0.0f;
    static constexpr float s_Cooldown = -0.0001f;

    TicTacToeMinimax m_Minimax;
    bool m_PlayAgainstMinimax = false;

    Elysium::Model m_TicTacToeModel;
    Elysium::Model m_TargetModel;
    std::vector<Elysium::Model> m_OpponentModels;
    int m_OpponentUpdates = 0;

    std::array<unsigned int, 4> m_ModelWins = { 0 };
    std::array<unsigned int, 4> m_ModelDraws = { 0 };
    std::array<unsigned int, 4> m_ModelGames = { 0 };
    
    size_t m_OpponentIndex = 0;
    size_t m_OpponentUpdateIndex = 0;
    size_t m_PositiveThreshold = 18; //18;
    size_t m_WinThreshold = 14; //14;
    std::array<bool, 4> m_OpponentUpdated = { false };

    bool m_TrainModel = false;
    bool m_PlayModel = false;
    bool m_DoneTraining = true;
    bool m_DoneSaving = false;

    Elysium::Matrix m_EpisodeData;

    bool m_Training = true;

    int m_PreviousTurn = 1;

    TicTacToeMinimax m_ModelMinimax;
    TicTacToeMinimax m_OpponentMinimax;
    TicTacToeMinimax::State m_RootState;
    TicTacToeMinimax::TreestrapData m_TreeData;

    size_t m_MoveCount = 0;

    size_t m_TargetUpdate = 0;
    static constexpr size_t s_UpdateTarget = 16;

    static constexpr size_t s_UpdateOpponent = 100;

    static constexpr size_t s_DataLength = 55;

    static constexpr size_t s_InputSize = 27;
    static constexpr size_t s_HiddenSize = 42;
    static constexpr size_t s_OutputSize = 1;

    static constexpr float s_LearningRate = 0.00001f; // 0.001f
    static constexpr float s_Beta = 5.0f;

    static constexpr float s_DiscountFactor = 0.9f;
    static constexpr size_t s_BatchSize = 32;
    static constexpr size_t s_MemorySize = 5000;

    static constexpr float s_TieReward = 0.0f;
    static constexpr float s_WinReward = 1.0f;
    static constexpr float s_LossReward = -1.0f;
    
    static constexpr int s_RateThreshold = 100;

    int m_Algorithm = 3; // 0 TD, 1 Monte Carlo, 2 TD Root, 3 Treestrap
    int m_Policy = 1; // 0 Greedy, 1 Softmax
    int m_Opponent = 1; // 0 Random, 1 Softmax, 2 Greedy

private:
    Elysium::Vector2 getPosition(Elysium::Action action);

    void addAction(Elysium::Vector2 position, size_t index);

    bool isWithinBounds(Elysium::Vector2 position, float x1, float y1, float x2, float y2);

    size_t TDRootAction(int turn);
    size_t TreestrapAction(int turn);
    size_t ModelAction(int turn);
    Elysium::Action chooseAction(int turn);
    size_t getRandomAction();
    size_t getMaxAction(const std::vector<float>& values);
    size_t getMinAction(const std::vector<float>& values);
    size_t getSoftmaxAction(const std::vector<float>& values, float beta);
    size_t getSoftminAction(const std::vector<float>& values, float beta);

    void trainModel(const TicTacToeGrid& previous, Elysium::Action action, int turn);
    void updateMonteCarloDataset(
        std::vector<float>& data,
        const std::vector<float>& previous, const std::vector<float>& state, float reward);
    void TD(const Elysium::Matrix& data);
    void TDRoot(const Elysium::Matrix& data);
    void Treestrap(const Elysium::Matrix& data);
    void MonteCarlo();

    void getState(const TicTacToeGrid& grid, std::vector<float>& state);
    void getInvertedState(const TicTacToeGrid& grid, std::vector<float>& state);

    void updateOpponent();
    void updateTarget();

    void modelGenerator(std::vector<TicTacToeMinimax::State>& states, const TicTacToeGrid& grid, int turn, size_t depth);
    void opponentGenerator(std::vector<TicTacToeMinimax::State>& states, const TicTacToeGrid& grid, int turn, size_t depth);

    void updateModels();
    void saveModels();

public:
    TicTacToeScene(unsigned int width, unsigned int height);
    ~TicTacToeScene();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onMousePressedEvent(Elysium::MouseButtonPressedEvent& event);
    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};