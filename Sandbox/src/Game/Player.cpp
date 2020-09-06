#include "Player.h"

#include "Values.h"

Player::Player() : m_RunAnimation(m_FrameRate)
{
    e_PhysicsSystem.getBodies().clear();
    m_Player = e_PhysicsSystem.createPhysicalBody(Elysium::BodyType::DYNAMIC, "Player", 50.0f, { -27.5f, 20.0f }, { 2.0f, 2.0f }, Player::onCollision);

    m_Player->setRadius(1.0f);
    m_Player->setNumberOfCallbackExecution(1);
}

void Player::onUpdate(Elysium::Timestep ts)
{
    if (Elysium::Input::isKeyPressed(ELY_KEY_A))
    {
        m_RunAnimation.updateAnimation(ts);
        if (m_PlayerLookingRight)
        {
            m_RunAnimation.reflectAroundYAxis();
            m_PlayerLookingRight = false;
        }
        m_Player->Impulse.x += -2.0f * m_Player->getMass() * ts;
        m_TextureData = m_RunAnimation.getCurrentTexture();
    }
    else if (Elysium::Input::isKeyPressed(ELY_KEY_D))
    {
        m_RunAnimation.updateAnimation(ts);
        if (!m_PlayerLookingRight)
        {
            m_RunAnimation.reflectAroundYAxis();
            m_PlayerLookingRight = true;
        }
        m_Player->Impulse.x += 2.0f * m_Player->getMass() * ts;
        m_TextureData = m_RunAnimation.getCurrentTexture();
    }
    else
    {
        m_RunAnimation.reset();
        m_TextureData = m_IdleTexture;
        if (!m_PlayerLookingRight)
            m_TextureData.reflectAroundYAxis();
    }
    m_Cooldown += ts;
}

void Player::kickBall(Elysium::BodyHandle handle)
{
    Elysium::PhysicalBody* body = e_PhysicsSystem.getPhysicalBody(handle);
    float distance = ((body->Position.x - m_Player->Position.x) * (body->Position.x - m_Player->Position.x)) +
        ((body->Position.y - m_Player->Position.y) * (body->Position.y - m_Player->Position.y));
    if (distance < SoccerRange)
    {
        if (m_Cooldown >= 0.0f)
        {
            Elysium::Vector2 direction = normalize(body->Position - m_Player->Position);
            if (Elysium::Input::isKeyPressed(ELY_KEY_S))
            {
                body->Impulse += KickImpulse * 2.0f * direction * body->getMass();
            }
            else if (Elysium::Input::isKeyPressed(ELY_KEY_Q))
            {
                m_Cooldown = -CooldownTime;
                body->Impulse += KickImpulse * Elysium::Vector2(direction.x, 0.70710678118f) * body->getMass();
            }
            else if (Elysium::Input::isKeyPressed(ELY_KEY_E))
            {
                m_Cooldown = -CooldownTime;
                body->Position.x += SwapBall * -direction.x;
            }
        }
    }
}

void Player::movePlayer(Elysium::PhysicalBody* body)
{
    float distance = ((body->Position.x - m_Player->Position.x) * (body->Position.x - m_Player->Position.x)) +
        ((body->Position.y - m_Player->Position.y) * (body->Position.y - m_Player->Position.y));
    if (distance < SoccerRange)
    {
        if (m_Cooldown >= 0.0f)
        {
            Elysium::Vector2 direction = normalize(body->Position - m_Player->Position);
            if (Elysium::Input::isKeyPressed(ELY_KEY_E))
            {
                m_Cooldown = -CooldownTime;
                body->Position.x += SwapPlayer * -direction.x;

            }
        }
    }
}

void Player::onCollision(Elysium::PhysicalBody& body, Elysium::PhysicalBody& collidee, const Elysium::CollisionInfo& info)
{
    if (info.CollisionInfoPair.first.Normal.y > 0.01f)
    {
        if (Elysium::Input::isKeyPressed(ELY_KEY_W))
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