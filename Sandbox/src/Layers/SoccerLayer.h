#pragma once

#include "Game/Adversary.h"
#include "Game/Player.h"

class SoccerLayer : public Elysium::Layer
{
private:
    bool* m_RunSandbox;

    float m_Height = 30.0f;

    Elysium::OrthographicCamera m_Camera;
    std::vector<Texture> m_Textures;
    TextureData m_Background;

    Player m_Player;
    Adversary m_Adversary;

    bool m_GameOver = false;
    uint32_t m_BlueScore = 0;
    uint32_t m_RedScore = 0;

    std::array<Elysium::BodyHandle, 3> m_Rectangles;

    Elysium::BodyHandle m_Ground;
    TextureData m_GroundTexture;
    Elysium::BodyHandle m_Ball;
    TextureData m_BallTexture;

public:
    SoccerLayer(bool* runSandbox, unsigned int height, unsigned int width);

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onKeyPressedEvent(Elysium::KeyPressedEvent& event);
    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};