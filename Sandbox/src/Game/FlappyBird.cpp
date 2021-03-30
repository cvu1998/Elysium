#include "FlappyBird.h"

FlappyBird::FlappyBird()
{
}

void FlappyBird::onUpdate(Elysium::Timestep ts)
{
    if (Body->CallbackExecutions > 0)
    {
        PauseBird = true;
    }
}

void FlappyBird::onEvent(Elysium::Event& event)
{
    Elysium::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Elysium::KeyPressedEvent>(BIND_EVENT_FUNCTION(FlappyBird::onKeyPressedEvent));
}

bool FlappyBird::onKeyPressedEvent(Elysium::KeyPressedEvent& event)
{
    if (event.getKeyCode() == ELY_KEY_SPACE && !PauseBird && !UseRLAgent)
    {
        Body->Velocity.y = 10.0f;
    }

    return false;
}

void FlappyBird::onCollision(Elysium::PhysicalBody2D& body, Elysium::PhysicalBody2D& collidee, const Elysium::CollisionInfo& info)
{
    body.CallbackExecutions++;
}