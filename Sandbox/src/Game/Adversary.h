#pragma once

#include "Game/Systems.h"

class Adversary
{
private:
    Elysium::PhysicalBody* m_Player;
    float m_Cooldown = 0.0f;

    static constexpr size_t s_RunAnimationSize = 8;

public:
    float m_FrameRate = 9.0f;
    bool m_PlayerLookingRight = true;
    Elysium::Animation<s_RunAnimationSize> m_RunAnimation;
    TextureData m_TextureData;
    TextureData m_IdleTexture;

public:
    Adversary();

    inline Elysium::PhysicalBody* getBody() { return m_Player; }

    void onUpdate(Elysium::Timestep ts);
    void kickBall(Elysium::BodyHandle handle);
    void movePlayer(Elysium::PhysicalBody* body);

    static void onCollision(Elysium::PhysicalBody& body, Elysium::PhysicalBody& collidee, const Elysium::CollisionInfo& info);
};