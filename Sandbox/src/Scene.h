#pragma once

#include "Elysium.h"

class Scene
{
protected:
    const char* m_DebugName;

public:
    Scene(const char* name = "Scene") : m_DebugName(name) {};
    virtual ~Scene() = default;

    virtual void Cleanup() { }
    virtual void onUpdate(Elysium::Timestep ts) = 0;
    virtual void onEvent(Elysium::Event& event) { }
};