#pragma once

#include "PhysicalObject.h"

namespace Elysium
{
    class StaticObject : public PhysicalObject
    {
    public:
        StaticObject(const char* name, const glm::vec2& initialPosition, const glm::vec2& size);

        virtual void onCollision() override;
        virtual void onUpdate(Timestep ts) override final;
    };
}

