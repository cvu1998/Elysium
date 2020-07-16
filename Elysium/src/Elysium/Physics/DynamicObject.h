#pragma once

#include "PhysicalObject.h"

namespace Elysium
{
    class DynamicObject : public PhysicalObject
    {
    protected:
        virtual void applyObjectLogic(Timestep ts) override;
        virtual void onUpdate(Timestep ts) override final;

    public:
        DynamicObject(const char* name, const glm::vec2& initialPosition, const glm::vec2& size, float mass);

        virtual void onCollision(const PhysicalObject* ObjectCollided, 
            const ObjectCollisionInfo& info, const ObjectCollisionInfo& otherInfo, Timestep ts) override;
    };
}