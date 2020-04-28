#pragma once

#include <string>

#include "Events/Event.h"

namespace Elysium
{
    class Layer
    {
    protected:
        std::string m_DebugName;

    public:
        Layer(const std::string& name = "Layer");
        virtual ~Layer();

        virtual void onAttach() = 0;
        virtual void onDetach() = 0;
        virtual void onUpdate() = 0;
        virtual void onEvent(Event& event) = 0;
    };
}


