#pragma once

#include "PhysicalObject.h"

namespace Elysium
{
    class DynamicObject : public PhysicalObject
    {
    protected:

    public:
        DynamicObject(const char* name, const glm::vec2& initialPosition, const glm::vec2& size, float mass);

        virtual void onCollision() override;
        virtual void onUpdate(Timestep ts) override;
    };
}