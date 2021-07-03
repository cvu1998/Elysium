#pragma once

#include "Game/DummyPlayer.h"

class SandboxScene : public Elysium::Scene
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

    Elysium::BodyHandle m_MoveableBox;
    Elysium::BodyHandle m_Ground;
    Elysium::TextureData m_GroundTexture;
    Elysium::BodyHandle m_Box;
    Elysium::BodyHandle m_sBox;
    Elysium::TextureData m_BoxTexture;
    Elysium::BodyHandle m_Ball;
    Elysium::BodyHandle m_Circle;
    Elysium::TextureData m_BallTexture;

public:
    SandboxScene(unsigned int height, unsigned int width);
    ~SandboxScene();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onKeyPressedEvent(Elysium::KeyPressedEvent& event);
    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};