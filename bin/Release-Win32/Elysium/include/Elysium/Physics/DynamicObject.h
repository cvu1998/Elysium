#pragma once

#include "PhysicalObject.h"

namespace Elysium
{
    class DynamicObject : public PhysicalObject
    {
    protected:

    public:
        DynamicObject(const glm::vec2& initialPosition, const glm::vec2& size, float mass);

        virtual glm::vec2 getFuturePosition(const glm::vec2& position, Timestep ts) const override final;
        virtual void onCollision() override;
        virtual void onUpdate(Timestep ts) override;
    };
}