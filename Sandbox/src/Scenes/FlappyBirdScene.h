#pragma once

#include "Game/FlappyBird.h"

static constexpr size_t FeatureVectorSize = 23;

class FlappyBirdAgent : public Elysium::LinearRLAgent<FeatureVectorSize>
{
private:
    float m_ExplorationProbability = 0.01f;

public:
    unsigned int Action = 0;
    bool NewEpisode = true;
    bool StopTraining = false;
    bool TrainAgent = true;

    bool isExploring = true;

    Elysium::BinaryFeatureVector<FeatureVectorSize> CurrentFeatureVector;
    Elysium::BinaryFeatureVector<FeatureVectorSize> NextFeatureVector;
    std::array<std::string, FeatureVectorSize> FeatureNotes;

public:
    FlappyBirdAgent() : Elysium::LinearRLAgent<FeatureVectorSize>(0.1f, 0.9f, 2.0f)
    {
        readWeightVectorFromFile("res/AI/FlappyBirdWeightVector.rl");
    }

    inline float getExplorationProbability() const { return m_ExplorationProbability; }

    void getStateActionFeatures(const Elysium::PhysicalBody2D* body,
        const Elysium::PhysicalBody2D* lowerPipe,
        const Elysium::PhysicalBody2D* upperPipe,
        Elysium::Action action, Elysium::BinaryFeatureVector<FeatureVectorSize>& stateActionFeatures)
    {
        float halfSizeLowerPipeY = lowerPipe->getSize().y * 0.5f;
        float halfSizeUpperPipeY = upperPipe->getSize().y * 0.5f;

        bool isBelowGap = body->Position.y < lowerPipe->Position.y + halfSizeLowerPipeY;
        bool isAboveGap = body->Position.y > upperPipe->Position.y - halfSizeUpperPipeY;

        bool AboveMiddle = body->Position.y > lowerPipe->Position.y + halfSizeLowerPipeY + 2.0f;

        bool isBodyInPartion1 = body->Position.y <= lowerPipe->Position.y + halfSizeLowerPipeY + 0.5f &&
            body->Position.y > lowerPipe->Position.y + halfSizeLowerPipeY;
        bool isBodyInPartion2 = body->Position.y <= lowerPipe->Position.y + halfSizeLowerPipeY + 0.75f &&
            body->Position.y > lowerPipe->Position.y + halfSizeLowerPipeY + 0.5f;
        bool isBodyInPartion3 = body->Position.y <= lowerPipe->Position.y + halfSizeLowerPipeY + 1.0f &&
            body->Position.y > lowerPipe->Position.y + halfSizeLowerPipeY + 0.75f;
        bool isBodyInPartion4 = body->Position.y <= lowerPipe->Position.y + halfSizeLowerPipeY + 1.5f &&
            body->Position.y > lowerPipe->Position.y + halfSizeLowerPipeY + 1.0f;
        bool isBodyInPartion5 = body->Position.y <= lowerPipe->Position.y + halfSizeLowerPipeY + 2.0f &&
            body->Position.y > lowerPipe->Position.y + halfSizeLowerPipeY + 1.5f;
        bool isBodyInPartion6 = body->Position.y <= lowerPipe->Position.y + halfSizeLowerPipeY + 2.5f &&
            body->Position.y > lowerPipe->Position.y + halfSizeLowerPipeY + 2.0f;
        bool isBodyInPartion7 = body->Position.y <= lowerPipe->Position.y + halfSizeLowerPipeY + 3.0f &&
            body->Position.y > lowerPipe->Position.y + halfSizeLowerPipeY + 2.5f;
        bool isBodyInPartion8 = body->Position.y <= lowerPipe->Position.y + halfSizeLowerPipeY + 3.5f &&
            body->Position.y > lowerPipe->Position.y + halfSizeLowerPipeY + 3.0f;
        bool isBodyInPartion9 = body->Position.y <= lowerPipe->Position.y + halfSizeLowerPipeY + 4.0f &&
            body->Position.y > lowerPipe->Position.y + halfSizeLowerPipeY + 3.5f;

        bool isBodyAbovePartion8 = body->Position.y > lowerPipe->Position.y + halfSizeLowerPipeY + 4.0f;
        bool isBodyBelowPartion1 = body->Position.y < lowerPipe->Position.y + halfSizeLowerPipeY + 0.5f;

        stateActionFeatures.Array =
        {
            action == 0 && isBodyInPartion1,
            action == 1 && isBodyInPartion1,
            action == 0 && isBodyInPartion2,
            action == 1 && isBodyInPartion2,
            action == 0 && isBodyInPartion3,
            action == 1 && isBodyInPartion3,
            action == 0 && isBodyInPartion4,
            action == 1 && isBodyInPartion4,
            action == 0 && isBodyInPartion5,
            action == 1 && isBodyInPartion5,
            action == 0 && isBodyInPartion6,
            action == 1 && isBodyInPartion6,
            action == 0 && isBodyInPartion7,
            action == 1 && isBodyInPartion7,
            action == 0 && isBodyInPartion8,
            action == 1 && isBodyInPartion8,
            action == 0 && isBodyInPartion9,
            action == 1 && isBodyInPartion9,

            action == 0 && isAboveGap,
            action == 1 && isAboveGap,
            action == 1 && AboveMiddle,
            action == 0 && isBelowGap,
            action == 1 && isBelowGap
        };

        FeatureNotes =
        {
            "Inside Partition 1 with Action 0",
            "Inside Partition 1 with Action 1",
            "Inside Partition 2 with Action 0",
            "Inside Partition 2 with Action 1",
            "Inside Partition 3 with Action 0",
            "Inside Partition 3 with Action 1",
            "Inside Partition 4 with Action 0",
            "Inside Partition 4 with Action 1",
            "Inside Partition 5 with Action 0",
            "Inside Partition 5 with Action 1",
            "Inside Partition 6 with Action 0",
            "Inside Partition 6 with Action 1",
            "Inside Partition 7 with Action 0",
            "Inside Partition 7 with Action 1",
            "Inside Partition 8 with Action 0",
            "Inside Partition 8 with Action 1",
            "Inside Partition 9 with Action 0",
            "Inside Partition 9 with Action 1",
            "Above Gap with Action 0",
            "Above Gap with Action 1",
            "Above Middle With Action 1",
            "Below Gap with Action 0",
            "Below Gap with Action 1"
        };
    }

    void chooseAction(float stayValue, float jumpValue)
    {
        if (jumpValue > stayValue)
            Action = 1;
        else
            Action = 0;
    }

    void printWeightVector()
    {
        ELY_INFO("Weight Vector: [---");
        for (size_t i = 0; i < WeightVector.size(); i++)
        {
            ELY_INFO("{0}: {1}", FeatureNotes[i], WeightVector[i]);
        }
        ELY_INFO("---]");
    }
};

class FlappyBirdScene : public Elysium::Scene
{
private:
    float m_Height = 30.0f;
    float m_Cooldown = 0.0f;
    bool m_GameOver = false;
    bool m_PauseScene = true;

    Elysium::OrthographicCamera m_Camera;
    Elysium::Texture m_SpriteSheet;

    Elysium::Vector2 m_InitialPosition = { 0.0f, 0.0f };
    FlappyBird m_Bird;
    Elysium::TextureData m_BirdSprite;
    float m_SavedPosition = 0.0f;
    float m_InitialImpulse = 5.0f;

    Elysium::PhysicalBody2D* m_Ground = nullptr;
    Elysium::TextureData m_GroundSprite;
    Elysium::PhysicalBody2D* m_Ceiling = nullptr;
    Elysium::TextureData m_CeilingSprite;

    Elysium::TextureData m_Background;
    std::deque<float>m_BackgroundPositions;
    float m_BackgroundPosition = 0.0f;

    Elysium::TextureData m_LowerSprite;
    Elysium::TextureData m_UpperSprite;
    std::deque<Elysium::BodyHandle> m_LowerPipes;
    std::deque<Elysium::BodyHandle> m_UpperPipes;
    Elysium::PhysicalBody2D* m_LowerPipe = nullptr;
    Elysium::PhysicalBody2D* m_UpperPipe = nullptr;
    size_t m_Score = 0;
    size_t m_PipeIndex = 0;
    size_t m_BestScore = 0;
    size_t m_NumberOfEpisodes = 1;

    float m_PipeDistance = 15.0f;

    FlappyBirdAgent m_Agent;

private:
    void generateRandomPipe(float xposition);
    
    void updateEnvironment(Elysium::Timestep ts);

    void chooseActionForAgent(Elysium::BinaryFeatureVector<FeatureVectorSize>& featureVector);

public:
    FlappyBirdScene(unsigned int width, unsigned int height);
    ~FlappyBirdScene();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};