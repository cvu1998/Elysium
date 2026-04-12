#pragma once

#include "../Scene.h"

#include "Game/DummyPlayer.h"

class SandboxScene : public Scene
{
private:
    float m_Height = 30.0f;

    Elysium::OrthographicCamera m_Camera;
    std::vector<Elysium::Texture> m_Textures;
    Elysium::TextureData m_Background;

    Elysium::ParticleProperties m_Particle;
    Elysium::ParticleProperties m_Particle2;

    DummyPlayer m_Player;

    Elysium::BodyID m_MoveableBox;
    Elysium::BodyID m_Ground;
    Elysium::BodyID m_Box1;
    Elysium::BodyID m_Box2;
    Elysium::BodyID m_Ball;
    Elysium::BodyID m_Circle;

    Elysium::TextureData m_GroundTexture;
    Elysium::TextureData m_BallTexture;
    Elysium::TextureData m_BoxTexture;

public:
    SandboxScene(unsigned int height, unsigned int width);
    ~SandboxScene();

    inline void Cleanup() override
    {
        Elysium::PhysicsSystem2D::Shutdown();
        Elysium::ParticleSystem2D::Shutdown();
    }

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onKeyPressedEvent(Elysium::KeyPressedEvent& event);
    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);

private:
    void getIrisData(Elysium::Matrix& irisInputs, Elysium::Matrix& irisOutputs);
};