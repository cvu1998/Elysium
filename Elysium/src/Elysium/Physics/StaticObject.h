#pragma once

#include "PhysicalObject.h"

namespace Elysium
{
    class StaticObject : public PhysicalObject
    {
    protected:
        virtual void applyObjectLogic(Timestep ts) override;
        virtual void onUpdate(Timestep ts) override final;

    public:
        StaticObject(const char* name, const glm::vec2& initialPosition, const glm::vec2& size);

        virtual void onCollision(const PhysicalObject* ObjectCollided, 
            const ObjectCollisionInfo& info, const ObjectCollisionInfo& otherInfo, Timestep ts) override;
    };
}

