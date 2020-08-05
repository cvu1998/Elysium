#pragma once

#include "Game/Player.h"

class PerformanceLayer : public Elysium::Layer
{
private:
    bool* m_RunGame;

    float m_Height = 30.0f;

    Elysium::OrthographicCamera m_Camera;
    std::vector<Texture> m_Textures;
    TextureData m_Background;

    Elysium::ParticleProperties m_Particle;
    Elysium::ParticleProperties m_Particle2;
    Elysium::ParticleSystem m_ParticleSystem;

    Player m_Player;

    TextureData m_GroundTexture;
    Elysium::BodyHandle m_Ground;
    std::array<Elysium::BodyHandle, 5000> m_GroundLayers;
    std::array<Elysium::BodyHandle, 250> m_Boxes;
    TextureData m_BoxTexture;
    TextureData m_BallTexture;

    size_t m_Index = 0;
    float m_SpawnTime = 0.0f;

public:
    PerformanceLayer(bool* runGame, unsigned int height, unsigned int width);

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onKeyPressedEvent(Elysium::KeyPressedEvent& event);
    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};