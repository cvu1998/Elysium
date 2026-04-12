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
    m_PlayerID = Elysium::PhysicsSystem2D::Get().createPhysicalBody(
        Elysium::BodyType::DYNAMIC,
        Elysium::Collider::QUAD,
        "Player",
        50.0f,
        position,
        { 2.0f, 2.0f },
        [this](Elysium::PhysicalBody2D& body, Elysium::PhysicalBody2D& collidee, const Elysium::CollisionInfo& info)
        {
            if (body.Velocity.y < 0.01f && info.CollisionInfoPair[0].Normal.y > 0.1f)
            {
                if (Elysium::Input::isKeyPressed(UpKey))
                {
                    body.Impulse.y += PLAYER_JUMP_SPEED * body.getMass();
                    body.CallbackExecutions++;
                }
            }
        }
    );

    getBody().setFrictionCoefficient(0.2f);
    getBody().AllowRotation = false;
    getBody().setNumberOfCallbackExecution(1);
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
        if (getBody().Velocity.x > -PLAYER_MAX_SPEED) getBody().Impulse.x += -PLAYER_SPEED * getBody().getMass() * ts;
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
        if (getBody().Velocity.x < PLAYER_MAX_SPEED) getBody().Impulse.x += PLAYER_SPEED * getBody().getMass() * ts;
        m_TextureData = RunAnimation.getCurrentTexture();
    }
    else
    {
        RunAnimation.reset();
        m_TextureData = m_IdleTexture;
        if (!m_PlayerLookingRight) m_TextureData.reflectAroundYAxis();
    }
}

void DummyPlayer::applyImpulseOnBall(Elysium::KeyPressedEvent& event) const
{
    if (glm::distance(getBody().Position, getBall().Position) < SoccerRange)
    {
        Elysium::Vector2 direction = normalize(getBall().Position - getBody().Position);
        if (event.getKeyCode() == KickKey)
        {
            getBall().Impulse += KickImpulse * BALL_SPEED * direction * getBall().getMass();
        }
        else if (event.getKeyCode() == LobKey)
        {
            getBall().Impulse += KickImpulse * Elysium::Vector2(direction.x, 0.70710678118f) * getBall().getMass();
        }
        else if (event.getKeyCode() == SwapKey)
        {
            getBall().Position.x += SwapBall * -direction.x;
        }
    }
}

void DummyPlayer::onEvent(Elysium::Event& event)
{
    Elysium::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Elysium::KeyPressedEvent>(BIND_EVENT_FUNCTION(DummyPlayer::onKeyPressedEvent));
}

bool DummyPlayer::onKeyPressedEvent(Elysium::KeyPressedEvent& event)
{
    if (m_BallID < std::numeric_limits<uint32_t>::max())
    {
        auto& physicsSys = Elysium::PhysicsSystem2D::Get();
        if (physicsSys.isRunningThread())
        {
            std::scoped_lock lock(physicsSys.getUpdateMutex());
            applyImpulseOnBall(event);
        }
        else
        {
            applyImpulseOnBall(event);
        }
    }
    return false;
}