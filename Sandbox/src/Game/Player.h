#pragma once

#include "Game/Systems.h"

class Player
{
private:
    Elysium::BodyHandle m_ID;

    static constexpr size_t s_RunAnimationSize = 8;

public:
    float m_FrameRate = 9.0f;
    bool m_PlayerLookingRight = true;
    Elysium::Animation<s_RunAnimationSize> m_RunAnimation;
    TextureData m_TextureData;
    TextureData m_IdleTexture; 

public:
    Player();

    inline Elysium::BodyHandle getIdentifier() const { return m_ID; }

    void onUpdate(Elysium::Timestep ts);

    static void onCollision(Elysium::PhysicalBody& body, Elysium::PhysicalBody& collidee, const Elysium::CollisionInfo& info);
};

