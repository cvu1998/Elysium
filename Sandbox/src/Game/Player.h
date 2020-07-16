#pragma once

#include <Elysium.h>

class Player : public Elysium::DynamicObject
{
private:
    bool m_PlayerLookingRight = true;

private:
    virtual void applyObjectLogic(Elysium::Timestep ts) override;

public:
    Player(const Elysium::Vector2& position, const Elysium::Vector2& size, float mass);

    virtual void onCollision(const PhysicalObject* ObjectCollided,
        const Elysium::ObjectCollisionInfo& info, const Elysium::ObjectCollisionInfo& otherInfo, Elysium::Timestep ts) override;
};

