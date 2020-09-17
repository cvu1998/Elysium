#pragma once

#include "Timestep.h"

#include "Elysium/Events/Event.h"

namespace Elysium
{
    class Layer
    {
    protected:
        const char* m_DebugName;

    public:
        Layer(const char* name = "Layer");
        virtual ~Layer();

        virtual void onAttach() { }
        virtual void onDetach() { }
        virtual void onUpdate(Timestep ts) = 0;
        virtual void onEvent(Event& event) { }
    };
}


