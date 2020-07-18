#include "Player.h"

Player::Player(Elysium::PhysicsSystem& system, const Elysium::Vector2& position, const Elysium::Vector2& size, float mass)
{
    m_ID = system.createPhysicalBody(Elysium::BodyType::DYNAMIC, "Player", mass, position, size, Player::onCollision);
}

void Player::onCollision(Elysium::PhysicalBody& body, const Elysium::CollisionInfo& info)
{
    if (info.CollisionInfoPair.first.Normal.y > 0.0f)
    {
        if (Elysium::Input::isKeyPressed(ELY_KEY_SPACE))
            body.Impulse.y = 10.0f * body.getMass();

        if (Elysium::Input::isKeyPressed(ELY_KEY_A))
            body.Impulse.x = -5.0f *body.getMass() * info.ts;
        else if (Elysium::Input::isKeyPressed(ELY_KEY_D))
            body.Impulse.x = 5.0f * body.getMass() * info.ts;
    }
}