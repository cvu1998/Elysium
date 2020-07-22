#pragma once

#include "Game/Systems.h"

class Player
{
private:
    Elysium::BodyHandle m_ID;

public:
    bool m_PlayerLookingRight = true;
    TextureData m_TextureData;

public:
    Player(const Elysium::Vector2& position, const Elysium::Vector2& size, float mass);

    inline Elysium::BodyHandle getIdentifier() const { return m_ID; }

    void onUpdate(Elysium::Timestep ts);

    static void onCollision(Elysium::PhysicalBody& body, const Elysium::CollisionInfo& info);
};

