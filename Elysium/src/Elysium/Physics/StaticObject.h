#pragma once

#include "PhysicalObject.h"

namespace Elysium
{
    class StaticObject : public PhysicalObject
    {
    public:
        StaticObject(glm::vec2&& initialPosition);

        virtual glm::vec2 getFuturePosition(Timestep ts) const final;
        virtual void onCollision() override;
        virtual void onUpdate(Timestep ts) override final;
    };
}

