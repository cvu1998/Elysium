#pragma once

#include "Game/DummyPlayer.h"

class PerformanceScene : public Elysium::Scene
{
private:
    float m_Height = 30.0f;

    Elysium::OrthographicCamera m_Camera;
    std::vector<Elysium::Texture> m_Textures;
    Elysium::TextureData m_Background;

    Elysium::ParticleProperties m_Particle;
    Elysium::ParticleProperties m_Particle2;
    Elysium::ParticleSystem m_ParticleSystem;

    DummyPlayer m_Player;

    Elysium::TextureData m_GroundTexture;
    Elysium::BodyHandle m_Ground;
    std::array<Elysium::BodyHandle, 5000> m_GroundLayers;
    std::array<Elysium::BodyHandle, 1000> m_Boxes;
    Elysium::TextureData m_BoxTexture;

    size_t m_Index = 0;
    float m_SpawnTime = 0.0f;

public:
    PerformanceScene(unsigned int height, unsigned int width);
    ~PerformanceScene();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};