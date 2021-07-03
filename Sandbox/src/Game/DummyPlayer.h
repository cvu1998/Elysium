#pragma once

#include "Game/Systems.h"

class DummyPlayer
{
private:
    Elysium::PhysicalBody2D* m_Player;

    static constexpr size_t s_RunAnimationSize = 8;

    int UpKey = ELY_KEY_W;
    int LeftKey = ELY_KEY_A;
    int RightKey = ELY_KEY_D;

    int KickKey = ELY_KEY_S;
    int LobKey = ELY_KEY_Q;
    int SwapKey = ELY_KEY_E;

public:
    float m_FrameRate = 9.0f;
    bool m_PlayerLookingRight = true;
    Elysium::Animation<s_RunAnimationSize> RunAnimation;
    Elysium::TextureData m_TextureData;
    Elysium::TextureData m_IdleTexture; 

    Elysium::PhysicalBody2D* Ball = nullptr;

public:
    DummyPlayer(const Elysium::Vector2& position, int up = ELY_KEY_W, int left = ELY_KEY_A, int right = ELY_KEY_D,
        int kick = ELY_KEY_S, int lob = ELY_KEY_Q, int swap = ELY_KEY_E);

    inline Elysium::PhysicalBody2D* getBody() { return m_Player; }

    void onUpdate(Elysium::Timestep ts);

    void onEvent(Elysium::Event& event);
    bool onKeyPressedEvent(Elysium::KeyPressedEvent& event);
};