#pragma once

#include "Game/Player.h"

class SandboxLayer : public Elysium::Layer
{
private:
    bool* m_RunSandbox;

    float m_Height = 30.0f;

    Elysium::OrthographicCamera m_Camera;
    std::vector<Texture> m_Textures;
    TextureData m_Background;

    Elysium::ParticleProperties m_Particle;
    Elysium::ParticleProperties m_Particle2;
    Elysium::ParticleSystem m_ParticleSystem;

    Player m_Player;

    Elysium::BodyHandle m_MoveableBox;
    Elysium::BodyHandle m_Ground;
    TextureData m_GroundTexture;
    Elysium::BodyHandle m_Box;
    Elysium::BodyHandle m_sBox;
    TextureData m_BoxTexture;
    Elysium::BodyHandle m_Ball;
    Elysium::BodyHandle m_Circle;
    TextureData m_BallTexture;

public:
    SandboxLayer(bool* runSandbox, unsigned int height, unsigned int width);

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onKeyPressedEvent(Elysium::KeyPressedEvent& event);
    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};