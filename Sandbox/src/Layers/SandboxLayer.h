#pragma once

#include <Elysium.h>

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

    Elysium::DynamicObject m_MoveableBox;
    Elysium::StaticObject m_Ground;
    Elysium::StaticObject m_Box;
    TextureData m_BoxTexture;
    Elysium::PhysicsSystem m_PhysicsSystem;

public:
    SandboxLayer(bool* runSandbox, unsigned int height, unsigned int width);
    ~SandboxLayer();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onKeyPressedEvent(Elysium::KeyPressedEvent& event);
    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};