#pragma once

#include "Timestep.h"

#include "Elysium/Events/Event.h"

namespace Elysium
{
    class Scene
    {
    protected:
        const char* m_DebugName;

    public:
        Scene(const char* name = "Scene");
        virtual ~Scene();

        virtual void onUpdate(Timestep ts) = 0;
        virtual void onEvent(Event& event) { }
    };
}