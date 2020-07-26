#include "Player.h"

Player::Player(const Elysium::Vector2& position, const Elysium::Vector2& size, float mass)
{
    e_PhysicsSystem.getBodies().clear();
    m_ID = e_PhysicsSystem.createPhysicalBody(Elysium::BodyType::DYNAMIC, "Player", mass, position, size, Player::onCollision);

    Elysium::PhysicalBody& player = e_PhysicsSystem.getPhysicalBody(m_ID);
    player.setRadius(1.0f);
    player.setNumberOfCallbackExecution(1);
}

void Player::onUpdate(Elysium::Timestep ts)
{
    Elysium::PhysicalBody& player = e_PhysicsSystem.getPhysicalBody(m_ID);

    if (Elysium::Input::isKeyPressed(ELY_KEY_A))
    {
        if (m_PlayerLookingRight)
        {
            m_TextureData.reflectAroundYAxis();
            m_PlayerLookingRight = false;
        }
        player.Impulse.x += -2.0f * player.getMass() * ts;
    }
    else if (Elysium::Input::isKeyPressed(ELY_KEY_D))
    {
        if (!m_PlayerLookingRight)
        {
            m_TextureData.reflectAroundYAxis();
            m_PlayerLookingRight = true;
        }
        player.Impulse.x += 2.0f * player.getMass() * ts;
    }
}

void Player::onCollision(Elysium::PhysicalBody& body, const Elysium::CollisionInfo& info)
{
    if (info.CollisionInfoPair.first.Normal.y > 0.01f)
    {
        if (Elysium::Input::isKeyPressed(ELY_KEY_SPACE))
        {
            body.Impulse.y += 10.0f * body.getMass();
            body.CallbackExecutions++;
        }

        if (Elysium::Input::isKeyPressed(ELY_KEY_A))
            body.Impulse.x += -40.0f * body.getMass()  * info.ts;
        else if (Elysium::Input::isKeyPressed(ELY_KEY_D))
            body.Impulse.x += 40.0f * body.getMass() * info.ts;
    }
}