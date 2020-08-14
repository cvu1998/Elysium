#pragma once

#include <Elysium.h>

#include "Game/Connect4Grid.h"

class Connect4Layer : public Elysium::Layer
{
private:
    bool* m_RunConnect4;

    float m_Height = 30.0f;

    Elysium::OrthographicCamera m_Camera;
    Texture m_SpriteSheet;

    std::array<Elysium::BodyHandle, 9> m_Rectangles;
    std::array<std::pair<Elysium::PhysicalBody*, uint32_t>, 42> m_Coins = { std::make_pair(nullptr, 0) };
    Elysium::BodyHandle m_CoinIndex = 0;
    std::array<TextureData, 2> m_CoinTextures; // 1 Blue, 2 Red

    Connect4Grid m_Grid;
    uint32_t m_Turn = 1;
    float m_MoveCooldown = 1.0f;
    bool m_GameOver = false;
    bool m_Restart = false;
    uint32_t m_BlueScore = 0;
    uint32_t m_RedScore = 0;

public:
    Connect4Layer(bool* runConnect4, unsigned int width, unsigned int height);
    ~Connect4Layer();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onMousePressedEvent(Elysium::MouseButtonPressedEvent& event);
    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};

