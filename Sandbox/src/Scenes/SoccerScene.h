#pragma once

#include "Game/DummyPlayer.h"

class SoccerScene : public Elysium::Scene
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

    std::array<Elysium::BodyHandle, 3> m_Rectangles;

    Elysium::BodyHandle m_Ground;
    Elysium::TextureData m_GroundTexture;
    Elysium::BodyHandle m_Ball;
    Elysium::TextureData m_BallTexture;

public:
    SoccerScene(unsigned int height, unsigned int width);
    ~SoccerScene();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onKeyPressedEvent(Elysium::KeyPressedEvent& event);
    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};