#pragma once

#include "PhysicalObject.h"

namespace Elysium
{
    class DynamicObject : public PhysicalObject
    {
    protected:
        float Mass;

    public:
        DynamicObject(glm::vec2&& initialPosition, float mass);

        virtual glm::vec2 getFuturePosition(Timestep ts) const override final;
        virtual void onCollision() override;
        virtual void onUpdate(Timestep ts) override;
    };
}