#pragma once

#include "Systems.h"

class FlappyBird
{
public:
    Elysium::PhysicalBody2D* Body = nullptr;
    bool UseRLAgent = true;
    bool PauseBird = false;
    Elysium::TextureData m_TextureData;

public:
    FlappyBird();

    void onUpdate(Elysium::Timestep ts);
    void onEvent(Elysium::Event& event);
    bool onKeyPressedEvent(Elysium::KeyPressedEvent& event);

    static void onCollision(Elysium::PhysicalBody2D& body, Elysium::PhysicalBody2D& collidee, const Elysium::CollisionInfo& info);
};