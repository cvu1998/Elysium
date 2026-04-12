#pragma once

#include "Elysium.h"

class DummyPlayer
{
private:
    Elysium::BodyID m_PlayerID;
    Elysium::BodyID m_BallID = std::numeric_limits<uint32_t>::max();

    static constexpr size_t s_RunAnimationSize = 8;

    int UpKey = ELY_KEY_W;
    int LeftKey = ELY_KEY_A;
    int RightKey = ELY_KEY_D;

    int KickKey = ELY_KEY_S;
    int LobKey = ELY_KEY_Q;
    int SwapKey = ELY_KEY_E;\

private:
    void applyImpulseOnBall(Elysium::KeyPressedEvent& event) const;

public:
    float m_FrameRate = 9.0f;
    bool m_PlayerLookingRight = true;
    Elysium::Animation<s_RunAnimationSize> RunAnimation;
    Elysium::TextureData m_TextureData;
    Elysium::TextureData m_IdleTexture; 

public:
    DummyPlayer(
        const Elysium::Vector2& position,
        int up = ELY_KEY_W,
        int left = ELY_KEY_A,
        int right = ELY_KEY_D,
        int kick = ELY_KEY_S,
        int lob = ELY_KEY_Q,
        int swap = ELY_KEY_E
    );

    Elysium::PhysicalBody2D& getBody() const { return Elysium::PhysicsSystem2D::Get().getBody(m_PlayerID); }
    Elysium::PhysicalBody2D& getBall() const { return Elysium::PhysicsSystem2D::Get().getBody(m_BallID); }

    void setBallID(Elysium::BodyID ballID) { m_BallID = ballID; }

    void onUpdate(Elysium::Timestep ts);

    void onEvent(Elysium::Event& event);
    bool onKeyPressedEvent(Elysium::KeyPressedEvent& event);
};