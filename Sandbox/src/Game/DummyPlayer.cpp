#include "DummyPlayer.h"

#include "Values.h"

constexpr float PLAYER_SPEED = 10.0f;
constexpr float PLAYER_JUMP_SPEED = 12.5f;
constexpr float PLAYER_MAX_SPEED = 15.0f;
constexpr float BALL_SPEED = 10.0f;

DummyPlayer::DummyPlayer(
    const Elysium::Vector2& position,
    int up,
    int left,
    int right,
    int kick,
    int lob,
    int swap
) : 
    UpKey(up),
    LeftKey(left),
    RightKey(right),
    KickKey(kick),
    LobKey(lob),
    SwapKey(swap),
    RunAnimation(m_FrameRate)
{
    m_Player = Elysium::PhysicsSystem2D::Get().createPhysicalBody(
        Elysium::BodyType::DYNAMIC,
        Elysium::Collider::QUAD,
        "Player",
        50.0f,
        position,
        { 2.0f, 2.0f },
        [this](Elysium::PhysicalBody2D& body, Elysium::PhysicalBody2D& collidee, const Elysium::CollisionInfo& info)
        {
            if (body.Velocity.y < Elysium::EPSILON && info.CollisionInfoPair[0].Normal.y > 0.1f)
            {
                if (Elysium::Input::isKeyPressed(UpKey))
                {
                    body.Impulse.y += PLAYER_JUMP_SPEED * body.getMass();
                    body.CallbackExecutions++;
                }
            }
        }
    );

    m_Player->setFrictionCoefficient(0.2f);
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
        if (m_Player->Velocity.x > -PLAYER_MAX_SPEED) m_Player->Impulse.x += -PLAYER_SPEED * m_Player->getMass() * ts;
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
        if (m_Player->Velocity.x < PLAYER_MAX_SPEED) m_Player->Impulse.x += PLAYER_SPEED * m_Player->getMass() * ts;
        m_TextureData = RunAnimation.getCurrentTexture();
    }
    else
    {
        RunAnimation.reset();
        m_TextureData = m_IdleTexture;
        if (!m_PlayerLookingRight) m_TextureData.reflectAroundYAxis();
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
        if (glm::distance(m_Player->Position, Ball->Position) < SoccerRange)
        {
            Elysium::Vector2 direction = normalize(Ball->Position - m_Player->Position);
            if (event.getKeyCode() == KickKey)
                Ball->Impulse += KickImpulse * BALL_SPEED * direction * Ball->getMass();
            else if (event.getKeyCode() == LobKey)
                Ball->Impulse += KickImpulse * Elysium::Vector2(direction.x, 0.70710678118f) * Ball->getMass();
            else if (event.getKeyCode() == SwapKey)
                Ball->Position.x += SwapBall * -direction.x;
        }
    }
    return false;
}