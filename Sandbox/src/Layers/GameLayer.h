#pragma once

#include <Elysium.h>

#include "Game/Player.h"

class GameLayer : public Elysium::Layer
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

    Elysium::PhysicsSystem m_PhysicsSystem;

    Player m_Player;

    Elysium::PhysicsBody m_MoveableBox;
    TextureData m_GroundTexture;
    Elysium::PhysicsBody m_Ground;
    std::array<Elysium::PhysicsBody, 1000> m_GroundLayers;
    Elysium::PhysicsBody m_Box;
    std::array<Elysium::PhysicsBody, 250> m_Boxes;
    TextureData m_BoxTexture;

    size_t m_Index = 0;
    float m_SpawnTime = 0.0f;

public:
    GameLayer(bool* runGame, unsigned int height, unsigned int width);
    ~GameLayer();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onKeyPressedEvent(Elysium::KeyPressedEvent& event);
    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};