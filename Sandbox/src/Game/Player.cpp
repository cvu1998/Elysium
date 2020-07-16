#include "Player.h"

Player::Player(const Elysium::Vector2& position, const Elysium::Vector2& size, float mass) :
    Elysium::DynamicObject("Player", position, size, mass)
{
}

void Player::applyObjectLogic(Elysium::Timestep ts)
{
    if (Elysium::Input::isKeyPressed(ELY_KEY_A))
    {
        if (m_PlayerLookingRight)
        {
            TextureData.reflectAroundYAxis();
            m_PlayerLookingRight = false;
        }
        Impulse.x = -1.0f * Mass * ts;
    }
    else if (Elysium::Input::isKeyPressed(ELY_KEY_D))
    {
        if (!m_PlayerLookingRight)
        {
            TextureData.reflectAroundYAxis();
            m_PlayerLookingRight = true;
        }
        Impulse.x = 1.0f * Mass * ts;
    }
}

void Player::onCollision(const PhysicalObject* ObjectCollided, const Elysium::ObjectCollisionInfo& info, const Elysium::ObjectCollisionInfo& otherInfo, Elysium::Timestep ts)
{
    if (0 == strcmp(ObjectCollided->getName(), "Box") 
        && otherInfo.Normal.y == 1.0f 
        && ObjectCollided->getVelocity().y < -0.5f)
    {
        Position = { 0.0f, -5.0f };
    }
    else
    {
        if (info.Normal.y > 0.0f)
        {
            if (Elysium::Input::isKeyPressed(ELY_KEY_SPACE))
                Impulse.y = 10.0f * Mass;

            if (Elysium::Input::isKeyPressed(ELY_KEY_A))
                Impulse.x = -5.0f * Mass * ts;
            else if (Elysium::Input::isKeyPressed(ELY_KEY_D))
                Impulse.x = 5.0f * Mass * ts;
        }
    }
}