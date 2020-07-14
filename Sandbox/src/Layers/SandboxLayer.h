#pragma once

#include <Elysium.h>

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

    Elysium::DynamicObject m_Player;
    bool m_PlayerLookingRight = true;

    Elysium::DynamicObject m_MoveableBox;
    Elysium::StaticObject m_Ground;
    Elysium::StaticObject m_Box;
    Elysium::PhysicsSystem m_PhysicsSystem;

public:
    SandboxLayer(bool* runSandbox, unsigned int height, unsigned int width);
    ~SandboxLayer();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onKeyPressedEvent(Elysium::KeyPressedEvent& event);
    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};