#pragma once

#include "../Scene.h"
#include "Game/DummyPlayer.h"

class SoccerScene : public Scene
{
private:
    float m_Height = 30.0f;

    Elysium::OrthographicCamera m_Camera;
    std::vector<Elysium::Texture> m_Textures;
    Elysium::TextureData m_Background;

    DummyPlayer m_Player;
    DummyPlayer m_Adversary;

    bool m_GameOver = false;
    uint32_t m_BlueScore = 0;
    uint32_t m_RedScore = 0;

    std::array<Elysium::BodyID , 3> m_Rectangles;

    Elysium::BodyID m_Ground;
    Elysium::BodyID m_Ball;

    Elysium::TextureData m_GroundTexture;
    Elysium::TextureData m_BallTexture;

public:
    SoccerScene(unsigned int height, unsigned int width);
    ~SoccerScene();

    inline void Cleanup() override
    {
        Elysium::PhysicsSystem2D::Shutdown();
    }

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onKeyPressedEvent(Elysium::KeyPressedEvent& event);
    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};