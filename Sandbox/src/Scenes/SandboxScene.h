#pragma once

#include "Game/Player.h"

class SandboxScene : public Elysium::Scene
{
private:
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

    std::string m_Messages[3] = { "Je suis Samuel", "He was here", "Well hello there!" };

    Elysium::UDPClient m_Client;
    Elysium::UDPServer m_Server;

public:
    SandboxScene(unsigned int height, unsigned int width);
    ~SandboxScene();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onKeyPressedEvent(Elysium::KeyPressedEvent& event);
    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};