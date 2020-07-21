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

    Elysium::PhysicsBody m_MoveableBox;
    Elysium::PhysicsBody m_Ground;
    TextureData m_GroundTexture;
    Elysium::PhysicsBody m_Box;
    TextureData m_BoxTexture;
    Elysium::PhysicsBody m_Ball;
    Elysium::PhysicsBody m_Circle;
    TextureData m_BallTexture;

public:
    SandboxLayer(bool* runSandbox, unsigned int height, unsigned int width);
    ~SandboxLayer();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onKeyPressedEvent(Elysium::KeyPressedEvent& event);
    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};