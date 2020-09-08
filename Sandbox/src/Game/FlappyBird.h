#pragma once

#include "Systems.h"

class FlappyBird
{
public:
    Elysium::PhysicalBody* Body = nullptr;
    bool PauseBird = false;
    TextureData m_TextureData;

public:
    FlappyBird();

    void onUpdate(Elysium::Timestep ts);
    void onEvent(Elysium::Event& event);
    bool onKeyPressedEvent(Elysium::KeyPressedEvent& event);

    static void onCollision(Elysium::PhysicalBody& body, Elysium::PhysicalBody& collidee, const Elysium::CollisionInfo& info);
};