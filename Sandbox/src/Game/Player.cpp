#include "Player.h"

Player::Player()
{
    e_PhysicsSystem.getBodies().clear();
    e_PhysicsSystem.createPhysicalBody(&m_ID, Elysium::BodyType::DYNAMIC, "Player", 50.0f, { 20.0f, 20.0f }, { 2.0f, 2.0f }, Player::onCollision);

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

void Player::onCollision(Elysium::PhysicalBody& body, Elysium::PhysicalBody& collidee, const Elysium::CollisionInfo& info)
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


    if (collidee.getName() == "Ball")
    {
        if (Elysium::Input::isMouseButtonPressed(ELY_MOUSE_BUTTON_1))
            collidee.Impulse += 20.0f * Elysium::Vector2(0.70710678118f, 0.70710678118f) * collidee.getMass() * info.CollisionInfoPair.second.Normal.x;
    }
}