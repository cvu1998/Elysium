#pragma once

#include "Game/FlappyBird.h"

class FlappyBirdScene : public Elysium::Scene
{
private:
    float m_Height = 30.0f;
    float m_Cooldown = 0.0f;
    bool m_GameOver = false;

    Elysium::OrthographicCamera m_Camera;
    Texture m_SpriteSheet;

    Elysium::Vector2 m_InitialPosition = { 0.0f, 0.0f };
    FlappyBird m_Bird;
    TextureData m_BirdSprite;
    float m_SavedPosition = 0.0f;
    float m_InitialImpulse = 5.0f;

    Elysium::PhysicalBody* m_Ground = nullptr;
    TextureData m_GroundSprite;
    TextureData m_Background;
    std::vector<float>m_BackgroundPositions;
    float m_BackgroundPosition = 0.0f;
    Elysium::PhysicalBody* m_Ceiling = nullptr;

    TextureData m_LowerSprite;
    TextureData m_UpperSprite;
    std::vector<Elysium::BodyHandle> m_LowerPipes;
    std::vector<Elysium::BodyHandle> m_UpperPipes;
    size_t m_PipeIndex = 0;

    float m_PipeDistance = 15.0f;

public:
    FlappyBirdScene(unsigned int width, unsigned int height);
    ~FlappyBirdScene();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};