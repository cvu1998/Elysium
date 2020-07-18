#pragma once

#include <Elysium.h>

class Player
{
private:
    Elysium::PhysicsBody m_ID;

public:
    bool m_PlayerLookingRight = true;
    TextureData m_TextureData;

public:
    Player(Elysium::PhysicsSystem& system, const Elysium::Vector2& position, const Elysium::Vector2& size, float mass);

    inline Elysium::PhysicsBody getIdentifier() const { return m_ID; }

    static void onCollision(Elysium::PhysicalBody& body, const Elysium::CollisionInfo& info);
};

