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
    Player();

    inline Elysium::BodyHandle getIdentifier() const { return m_ID; }

    void onUpdate(Elysium::Timestep ts);

    static void onCollision(Elysium::PhysicalBody& body, Elysium::PhysicalBody& collidee, const Elysium::CollisionInfo& info);
};

