#pragma once

#include "Game/FlappyBird.h"

static constexpr size_t FeatureSize = 14;

class FlappyBirdAgent : public Elysium::LinearRLAgent<FeatureSize>
{
public:
    unsigned int Action = 0;
    bool NewEpisode = true;
    bool TrainAgent = true;

    Elysium::BinaryFeatureVector<FeatureSize> CurrentFeatureVector;
    std::array<std::string, FeatureSize> FeatureNotes;

public:
    FlappyBirdAgent() : Elysium::LinearRLAgent<FeatureSize>(5, 0.001f, 1.0f, 5.0f)
    {
        readWeightVectorFromFile("res/AI/FlappyBirdWeightVector.rl");
    }

    void getStateActionFeatures(const Elysium::PhysicalBody2D* body, 
        const Elysium::PhysicalBody2D* lowerPipe, 
        const Elysium::PhysicalBody2D* upperPipe,
        Elysium::Action action, std::array<float, FeatureSize>& stateActionFeatures)
    {
        float halfSizeLowerPipeY = lowerPipe->getSize().y * 0.5f;
        float halfSizeUpperPipeY = upperPipe->getSize().y * 0.5f;

        bool isBodyInPartion1 = body->Position.y <= lowerPipe->Position.y + halfSizeLowerPipeY + 1.0f;
        bool isBodyInPartion2 = body->Position.y <= lowerPipe->Position.y + halfSizeLowerPipeY + 2.0f && !isBodyInPartion1;
        bool isBodyInPartion3 = body->Position.y <= lowerPipe->Position.y + halfSizeLowerPipeY + 3.0f && !isBodyInPartion2;
        bool isBodyInPartion4 = body->Position.y <= lowerPipe->Position.y + halfSizeLowerPipeY + 4.0f && !isBodyInPartion3;
        bool isBodyInPartion5 = body->Position.y <= lowerPipe->Position.y + halfSizeLowerPipeY + 5.0f && !isBodyInPartion4;

        bool isBelowGap = body->Position.y <= lowerPipe->Position.y + halfSizeLowerPipeY;
        bool isAboveGap = body->Position.y >= upperPipe->Position.y - halfSizeUpperPipeY;

        size_t index = 0;
        stateActionFeatures[index++] = action == 0 && isBodyInPartion1 ? 1.0f : 0.0f;
        stateActionFeatures[index++] = action == 1 && isBodyInPartion1 ? 1.0f : 0.0f;
        stateActionFeatures[index++] = action == 0 && isBodyInPartion2 ? 1.0f : 0.0f;
        stateActionFeatures[index++] = action == 1 && isBodyInPartion2 ? 1.0f : 0.0f;
        stateActionFeatures[index++] = action == 0 && isBodyInPartion3 ? 1.0f : 0.0f;
        stateActionFeatures[index++] = action == 1 && isBodyInPartion3 ? 1.0f : 0.0f;
        stateActionFeatures[index++] = action == 0 && isBodyInPartion4 ? 1.0f : 0.0f;
        stateActionFeatures[index++] = action == 1 && isBodyInPartion4 ? 1.0f : 0.0f;
        stateActionFeatures[index++] = action == 0 && isBodyInPartion5 ? 1.0f : 0.0f;
        stateActionFeatures[index++] = action == 1 && isBodyInPartion5 ? 1.0f : 0.0f;

        stateActionFeatures[index++] = action == 0 && isAboveGap ? 1.0f : 0.0f;
        stateActionFeatures[index++] = action == 1 && isAboveGap ? 1.0f : 0.0f;
        stateActionFeatures[index++] = action == 0 && isBelowGap ? 1.0f : 0.0f;
        stateActionFeatures[index++] = action == 1 && isBelowGap ? 1.0f : 0.0f;

        #ifdef _DEBUG
        index = 0;
        FeatureNotes[index++] = "Inside Gap 1 with Action 0";
        FeatureNotes[index++] = "Inside Gap 1 with Action 1";
        FeatureNotes[index++] = "Inside Gap 2 with Action 0";
        FeatureNotes[index++] = "Inside Gap 2 with Action 1";
        FeatureNotes[index++] = "Inside Gap 3 with Action 0";
        FeatureNotes[index++] = "Inside Gap 3 with Action 1";
        FeatureNotes[index++] = "Inside Gap 4 with Action 0";
        FeatureNotes[index++] = "Inside Gap 4 with Action 1";
        FeatureNotes[index++] = "Inside Gap 5 with Action 0";
        FeatureNotes[index++] = "Inside Gap 5 with Action 1";

        FeatureNotes[index++] = "Above Gap with Action 0";
        FeatureNotes[index++] = "Above Gap with Action 1";
        FeatureNotes[index++] = "Below Gap with Action 0";
        FeatureNotes[index++] = "Below Gap with Action 1";
        #endif
    }

    void chooseAction(float stayValue, float jumpValue)
    {
        if (jumpValue > stayValue)
        {
            Action = 1;
        }
        else
        {
            Action = 0;
        }
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

    void chooseActionForAgent(Elysium::BinaryFeatureVector<FeatureSize>& featureVector);

public:
    FlappyBirdScene(unsigned int width, unsigned int height);
    ~FlappyBirdScene();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};