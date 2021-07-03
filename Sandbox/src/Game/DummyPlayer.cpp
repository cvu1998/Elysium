#include "DummyPlayer.h"

#include "Values.h"

DummyPlayer::DummyPlayer(
    const Elysium::Vector2& position, 
    int up, int left, int right,
    int kick, int lob, int swap) : 
    UpKey(up), LeftKey(left), RightKey(right),
    KickKey(kick), LobKey(lob), SwapKey(swap),
    RunAnimation(m_FrameRate)
{
    m_Player = e_PhysicsSystem2D.createPhysicalBody(Elysium::BodyType::DYNAMIC, Elysium::Collider::QUAD,
        "Player", 50.0f, position, { 2.0f, 2.0f },
        [this](Elysium::PhysicalBody2D& body, Elysium::PhysicalBody2D& collidee, const Elysium::CollisionInfo& info)
        {
            if (info.CollisionInfoPair.first.Normal.y > 0.01f)
            {
                if (Elysium::Input::isKeyPressed(UpKey))
                {
                    body.Impulse.y += 10.0f * body.getMass();
                    body.CallbackExecutions++;
                }

                if (Elysium::Input::isKeyPressed(LeftKey))
                    body.Impulse.x += -40.0f * body.getMass() * info.ts;
                else if (Elysium::Input::isKeyPressed(RightKey))
                    body.Impulse.x += 40.0f * body.getMass() * info.ts;
            }
        });

    m_Player->AllowRotation = false;
    m_Player->setNumberOfCallbackExecution(1);
}

void DummyPlayer::onUpdate(Elysium::Timestep ts)
{
    if (Elysium::Input::isKeyPressed(LeftKey))
    {
        RunAnimation.updateAnimation(ts);
        if (m_PlayerLookingRight)
        {
            RunAnimation.reflectAroundYAxis();
            m_PlayerLookingRight = false;
        }
        m_Player->Impulse.x += -2.0f * m_Player->getMass() * ts;
        m_TextureData = RunAnimation.getCurrentTexture();
    }
    else if (Elysium::Input::isKeyPressed(RightKey))
    {
        RunAnimation.updateAnimation(ts);
        if (!m_PlayerLookingRight)
        {
            RunAnimation.reflectAroundYAxis();
            m_PlayerLookingRight = true;
        }
        m_Player->Impulse.x += 2.0f * m_Player->getMass() * ts;
        m_TextureData = RunAnimation.getCurrentTexture();
    }
    else
    {
        RunAnimation.reset();
        m_TextureData = m_IdleTexture;
        if (!m_PlayerLookingRight)
            m_TextureData.reflectAroundYAxis();
    }
}

void DummyPlayer::onEvent(Elysium::Event& event)
{
    Elysium::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Elysium::KeyPressedEvent>(BIND_EVENT_FUNCTION(DummyPlayer::onKeyPressedEvent));
}

bool DummyPlayer::onKeyPressedEvent(Elysium::KeyPressedEvent& event)
{
    if (Ball)
    {
        float distance = glm::distance(m_Player->Position, Ball->Position);

        if (distance < SoccerRange)
        {
            Elysium::Vector2 direction = normalize(Ball->Position - m_Player->Position);
            if (event.getKeyCode() == KickKey)
                Ball->Impulse += KickImpulse * 2.0f * direction * Ball->getMass();
            else if (event.getKeyCode() == LobKey)
                Ball->Impulse += KickImpulse * Elysium::Vector2(direction.x, 0.70710678118f) * Ball->getMass();
            else if (event.getKeyCode() == SwapKey)
                Ball->Position.x += SwapBall * -direction.x;
        }
    }

    return false;
}