#include "Adversary.h"

Adversary::Adversary() : m_RunAnimation(m_FrameRate)
{
    m_Player = e_PhysicsSystem.createPhysicalBody(Elysium::BodyType::DYNAMIC, "Adversary", 50.0f, { 27.5f, 20.0f }, { 2.0f, 2.0f }, Adversary::onCollision);

    m_Player->setRadius(1.0f);
    m_Player->setNumberOfCallbackExecution(1);
}

void Adversary::onUpdate(Elysium::Timestep ts)
{
    if (Elysium::Input::isKeyPressed(ELY_KEY_LEFT))
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
    else if (Elysium::Input::isKeyPressed(ELY_KEY_RIGHT))
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
}

void Adversary::kickBall(Elysium::BodyHandle handle)
{
    Elysium::PhysicalBody* body = e_PhysicsSystem.getPhysicalBody(handle);
    float distance = ((body->Position.x - m_Player->Position.x) * (body->Position.x - m_Player->Position.x)) +
        ((body->Position.y - m_Player->Position.y) * (body->Position.y - m_Player->Position.y));
    if (distance < 6.0f)
    {
        Elysium::Vector2 direction = normalize(body->Position - m_Player->Position);
        if (Elysium::Input::isKeyPressed(ELY_KEY_DOWN))
        {
            body->Impulse += 10.0f * direction * body->getMass();
        }
        else if (Elysium::Input::isKeyPressed(ELY_KEY_RIGHT_SHIFT))
        {
            body->Impulse += 7.5f * Elysium::Vector2(direction.x, 0.70710678118f) * body->getMass();
        }
    }
}

void Adversary::onCollision(Elysium::PhysicalBody& body, Elysium::PhysicalBody& collidee, const Elysium::CollisionInfo& info)
{
    if (info.CollisionInfoPair.first.Normal.y > 0.01f)
    {
        if (Elysium::Input::isKeyPressed(ELY_KEY_UP))
        {
            body.Impulse.y += 10.0f * body.getMass();
            body.CallbackExecutions++;
        }

        if (Elysium::Input::isKeyPressed(ELY_KEY_LEFT))
            body.Impulse.x += -40.0f * body.getMass() * info.ts;
        else if (Elysium::Input::isKeyPressed(ELY_KEY_RIGHT))
            body.Impulse.x += 40.0f * body.getMass() * info.ts;
    }
}