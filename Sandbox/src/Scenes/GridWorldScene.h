#pragma once

#include <Elysium.h>

class GridWorldScene : public Elysium::Scene
{
public:
    GridWorldScene(unsigned int width, unsigned int height);
    ~GridWorldScene();

    void onUpdate(Elysium::Timestep ts) override;

private:
    using Grid = std::array<float, 9>;

    bool isValid(size_t action);

    void loadDataset(const char* filepath);
    void saveDataset(const char* filepath);

    size_t chooseAction();
    size_t getMaxAction(const Elysium::Matrix& QValues);
    size_t getSoftmaxAction(const Elysium::Matrix& QValues);
    void trainModel(const std::vector<float>& previous, size_t action);
    void updateDQNDataset(
        std::vector<float>& data,
        const std::vector<float>& previous, const std::vector<float>& state, size_t action, float reward);
    void updateMonteCarloDataset(
        std::vector<float>& data,
        const std::vector<float>& previous, const std::vector<float>& state, size_t action, float reward);
    void DQN();
    void modifiedDQN(const std::vector<float>& data);
    void MonteCarlo();

    void updateTarget();

    void printStats();

private:
    Elysium::OrthographicCamera m_Camera;

    static constexpr float s_Height = 30.0f;

    bool m_Pause = false;

    Grid m_Grid = { 0 };
    std::array<Elysium::Vector2, 9> m_GridCoordinates;

    Elysium::Texture m_FireSprites;
    Elysium::Texture m_GoldSprites;
    Elysium::Texture m_CatSprites;

    Elysium::TextureData m_FireTexture;
    Elysium::TextureData m_GoldTexture;
    Elysium::TextureData m_CatTexture;

    std::array<int, 4> m_Moves = { -1, -3, +1, +3 };

    float m_MoveCooldown = 0.0f;

    static constexpr float s_Cooldown = -0.05f;

    static constexpr size_t s_Goal = 8;
    static constexpr size_t s_Fire1 = 6;
    static constexpr size_t s_Fire2 = 5;
    static constexpr size_t s_Start = 0;

    size_t m_CatPosition = 0;

    bool m_GameOver = false;
    size_t m_Games = 0;
    size_t m_Wins = 0;
    size_t m_HundoWins = 0;
    float m_WinRate = 0.0f;

    int m_Algorithm = 0; // 0 DQN, 1 Modified DQN, 2 Monte Carlo

    int m_Policy = 0; // 0 Greedy, 1 Softmax

    Elysium::Model m_GridModel;
    Elysium::Model m_TargetModel;

    Elysium::Matrix m_Dataset;
    Elysium::Matrix m_EpisodeData;
    std::vector<int> m_TerminalStates;

    bool m_DoneTraining = true;

    size_t m_TargetUpdate = 0;
    static constexpr size_t s_Update = 8;

    static constexpr size_t s_InputSize = 9;
    static constexpr size_t s_HiddenSize = 9;
    static constexpr size_t s_OutputSize = 4;

    static constexpr size_t s_DataLength = 20;

    static constexpr float s_DiscountFactor = 0.9f;
    static constexpr size_t s_BatchSize = 32;
};