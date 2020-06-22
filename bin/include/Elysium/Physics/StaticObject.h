#pragma once

#include "PhysicalObject.h"

namespace Elysium
{
    class StaticObject : public PhysicalObject
    {
    public:
        StaticObject(const glm::vec2& initialPosition, const glm::vec2& size);

        virtual glm::vec2 getFuturePosition(const glm::vec2& position, Timestep ts) const override final;
        virtual void onCollision() override;
        virtual void onUpdate(Timestep ts) override final;
    };
}

