#include "PhysicsSystem2D.h"

#include <algorithm>
#include <array>
#include <execution>
#include <string.h>

#define NOMINMAX
#include "Elysium/Application.h"
#include "Elysium/Log.h"
#include "Elysium/MathUtility.h"

namespace Elysium
{
    static PhysicsSystem2D* s_Instance = nullptr;

    constexpr float CORRECTION_FACTOR = 0.75f;

    PhysicsSystem2D::PhysicsSystem2D(bool useThread) :
        m_RunUpdateThread(useThread)
    {
        if (useThread) m_UpdateThread = std::move(std::thread(&PhysicsSystem2D::Update, this));
    }

    PhysicsSystem2D::~PhysicsSystem2D()
    {
        if (m_UpdateThread.joinable())
        {
            m_UpdateThread.join();
        }
    }

    template<>
    void PhysicsSystem2D::Init<UpdateDevice::CPU>()
    {
        if (!s_Instance) s_Instance = new PhysicsSystem2D(false);
    }

    template<>
    void PhysicsSystem2D::Init<UpdateDevice::CPU_THREAD>()
    {
        if (!s_Instance) s_Instance = new PhysicsSystem2D(true);
    }

    void PhysicsSystem2D::Shutdown()
    {
        s_Instance->m_Shutdown.store(true, std::memory_order_relaxed);
        delete s_Instance;
        s_Instance = nullptr;
    }

    PhysicsSystem2D& PhysicsSystem2D::Get()
    {
        return *s_Instance;
    }

    PhysicalBody2D* PhysicsSystem2D::createPhysicalBody(
        BodyType type,
        Collider collider,
        const char* name,
        float mass,
        const Vector2& initialPosition,
        const Vector2& size, 
        PhysicalBody2D::Collision_Callback&& callback
    )
    {
        if (m_InactiveBodies.empty())
        {
            m_Bodies.push_back(std::move(PhysicalBody2D(type, collider, name, mass, initialPosition, size, std::move(callback))));
            return &m_Bodies[m_Bodies.size() - 1];
        }
        BodyHandle body = m_InactiveBodies.back();
        m_InactiveBodies.pop_back();
        m_Bodies[body] = PhysicalBody2D(type, collider, name, mass, initialPosition, size, std::move(callback));
        return &m_Bodies[body];
    }

    void PhysicsSystem2D::createPhysicalBody(
        BodyHandle* handle,
        BodyType type,
        Collider collider,
        const char* name,
        float mass,
        const Vector2& initialPosition,
        const Vector2& size,
        PhysicalBody2D::Collision_Callback&& callback
    )
    {
        if (m_InactiveBodies.empty())
        {
            m_Bodies.push_back(std::move(PhysicalBody2D(type, collider, name, mass, initialPosition, size, std::move(callback))));
            *handle = (BodyHandle)m_Bodies.size() - 1;
            return;
        }
        BodyHandle body = m_InactiveBodies.back();
        m_InactiveBodies.pop_back();
        m_Bodies[body] = PhysicalBody2D(type, collider, name, mass, initialPosition, size, std::move(callback));
        *handle = body;
    }

    void PhysicsSystem2D::removePhysicalBody(BodyHandle body)
    {
        m_Bodies[body].Status = BodyStatus::INACTIVE;
        m_InactiveBodies.push_back(body);
        std::sort(m_InactiveBodies.begin(), m_InactiveBodies.end(), std::greater());
    }

    bool PhysicsSystem2D::checkBroadPhase(const PhysicalBody2D& body1, const PhysicalBody2D& body2)
    {
        Vector2 min1 = body1.MinimumVertex;
        Vector2 max1 = body1.MaximumVertex;
        Vector2 min2 = body2.MinimumVertex;
        Vector2 max2 = body2.MaximumVertex;

        bool CollisionX = max1.x >= min2.x && min1.x <= max2.x;
        bool CollisionY = max1.y >= min2.y && min1.y <= max2.y;
        return CollisionX && CollisionY;
    }

    void PhysicsSystem2D::checkNarrowPhase(const PhysicalBody2D& body1, const PhysicalBody2D& body2, CollisionInfo& info)
    {
        int result = static_cast<int>(body1.Model) | static_cast<int>(body2.Model);
        switch (result)
        {
        // CIRCLE-CIRCLE COLLISION
        case 2:
        {
            float radius1 = body1.Size.x * 0.5f;
            float radius2 = body2.Size.x * 0.5f;
            float distance = (body2.Position.x - body1.Position.x) *
                (body2.Position.x - body1.Position.x) +
                (body2.Position.y - body1.Position.y) *
                (body2.Position.y - body1.Position.y);
            if (distance < (radius2 + radius1) * (radius2 + radius1))
            {
                info.Collision = true;

                Vector2 normal = glm::normalize(body2.Position - body1.Position);
                info.CollisionInfoPair[1].Normal = normal;
                info.CollisionInfoPair[0].Normal = -normal;

                float min1 = glm::dot(body1.Position - (normal * radius1), normal);
                float max1 = glm::dot(body1.Position + (normal * radius1), normal);
                float min2 = glm::dot(body2.Position - (normal * radius2), normal);
                float max2 = glm::dot(body2.Position + (normal * radius2), normal);

                info.minOverlap = glm::min(max1, max2) - glm::max(min1, min2);
            }
            break;
        }
        // QUAD-QUAD COLLISION
        case 1:
        {
            std::array<Vector2, 4> vertices1;
            std::array<Vector2, 4> vertices2;
            body1.getVertices(vertices1);
            body2.getVertices(vertices2);

            // Max 4 normals per quad; stack array avoids per-call heap allocation
            Vector2 normals[8];
            int normalsCount = 0;
            for (const Vector2& n : body1.Normals) normals[normalsCount++] = n;
            for (const Vector2& n : body2.Normals) normals[normalsCount++] = n;

            float overlap = 0.0f;
            Vector2 collisionNormal = { 0.0f, 0.0f };
            bool firstIsMax = false;
            for (int k = 0; k < normalsCount; ++k)
            {
                const Vector2& normal = normals[k];
                float min1 = std::numeric_limits<float>::max();
                float max1 = -std::numeric_limits<float>::max();
                for (const Vector2& vertex : vertices1)
                {
                    float projection = dot(vertex, normal);
                    min1 = std::min(min1, projection);
                    max1 = std::max(max1, projection);
                }

                float min2 = std::numeric_limits<float>::max();
                float max2 = -std::numeric_limits<float>::max();
                for (const Vector2& vertex : vertices2)
                {
                    float projection = dot(vertex, normal);
                    min2 = std::min(min2, projection);
                    max2 = std::max(max2, projection);
                }

                if (!(max2 >= min1 && max1 >= min2)) return;

                overlap = std::min(max1, max2) - std::max(min1, min2);
                if (overlap < info.minOverlap)
                {
                    collisionNormal = normal;
                    info.minOverlap = overlap;
                    firstIsMax = max1 > max2;
                }
            }

            info.Collision = true;
            info.CollisionInfoPair[0].Normal = -collisionNormal;
            info.CollisionInfoPair[1].Normal = collisionNormal;
            if (firstIsMax)
            {
                info.CollisionInfoPair[0].Normal = collisionNormal;
                info.CollisionInfoPair[1].Normal = -collisionNormal;
            }
            break;
        }
        // CIRCLE-QUAD COLLISION
        case 3:
        {
            bool circleFirst = true;
            const PhysicalBody2D* circlePtr = &body1;
            const PhysicalBody2D* quadPtr = &body2;
            if (body2.Model == Collider::CIRCLE)
            {
                circleFirst = false;
                circlePtr = &body2;
                quadPtr = &body1;
            }

            std::array<Vector2, 4> vertices;
            quadPtr->getVertices(vertices);

            Vector2 normals[4];
            int normalsCount = 0;
            for (const Vector2& n : quadPtr->Normals) normals[normalsCount++] = n;

            float overlap = 0.0f;
            Vector2 collisionNormal = { 0.0f, 0.0f };
            bool firstIsMax = false;
            for (int k = 0; k < normalsCount; ++k)
            {
                const Vector2& normal = normals[k];
                Vector2 circleVertex = normal * circlePtr->Size.x * 0.5f;
                float min1 = dot(circlePtr->Position - circleVertex, normal);
                float max1 = dot(circlePtr->Position + circleVertex, normal);

                float min2 = std::numeric_limits<float>::max();
                float max2 = -std::numeric_limits<float>::max();
                for (const Vector2& vertex : vertices)
                {
                    float projection = dot(vertex, normal);
                    min2 = std::min(min2, projection);
                    max2 = std::max(max2, projection);
                }

                if (!(max2 >= min1 && max1 >= min2)) return;

                overlap = std::min(max1, max2) - std::max(min1, min2);
                if (overlap < info.minOverlap)
                {
                    collisionNormal = normal;
                    info.minOverlap = overlap;
                    firstIsMax = max1 > max2;
                }
            }

            info.Collision = true;
            info.CollisionInfoPair[1].Normal = collisionNormal;
            info.CollisionInfoPair[0].Normal = -collisionNormal;
            if ((firstIsMax && circleFirst) || (!firstIsMax && !circleFirst))
            {
                info.CollisionInfoPair[0].Normal = collisionNormal;
                info.CollisionInfoPair[1].Normal = -collisionNormal;
            }
            break;
        }
        }
    }

    void PhysicsSystem2D::printInfo(BodyHandle i)
    {
        ELY_CORE_TRACE("-----Body Tag: {0}-----", m_Bodies[i].Tag);
        ELY_CORE_TRACE("Position: {0}, {1}", m_Bodies[i].Position.x, m_Bodies[i].Position.y);
        ELY_CORE_TRACE("Velocity: {0}, {1}", m_Bodies[i].Velocity.x, m_Bodies[i].Velocity.y);
        ELY_CORE_TRACE("Acceleration: {0}, {1}", m_Bodies[i].Acceleration.x, m_Bodies[i].Acceleration.y);
        ELY_CORE_TRACE("Rotation: {0}", m_Bodies[i].Rotation);
        ELY_CORE_TRACE("Force: {0}, {1}", m_Bodies[i].Force.x, m_Bodies[i].Force.y);
        ELY_CORE_TRACE("Contact Impulse: {0}, {1}", m_Bodies[i].ContactImpulse.x, m_Bodies[i].ContactImpulse.y);
        ELY_CORE_TRACE("Impulse: {0}, {1}", m_Bodies[i].Impulse.x, m_Bodies[i].Impulse.y);
        ELY_CORE_TRACE("Contact Normal: {0}, {1}", m_Bodies[i].ContactNormal.x, m_Bodies[i].ContactNormal.y);
        ELY_CORE_TRACE("Normal: {0}, {1}", m_Bodies[i].Normal.x, m_Bodies[i].Normal.y);
        ELY_CORE_TRACE("-----End-----");
    }

    Vector2 PhysicsSystem2D::computeContactPoint(
        const PhysicalBody2D& bodyI,
        const PhysicalBody2D& bodyJ,
        const Vector2& iNormal)
    {
        int result = static_cast<int>(bodyI.Model) | static_cast<int>(bodyJ.Model);
        switch (result)
        {
        // CIRCLE-CIRCLE: contact on body i surface toward body j
        case 2:
            return bodyI.Position - iNormal * (bodyI.Size.x * 0.5f);

        // QUAD-QUAD: deepest vertex of body j into body i
        // (vertex of j with maximum projection along iNormal)
        case 1:
        {
            std::array<Vector2, 4> vJ;
            bodyJ.getVertices(vJ);
            Vector2 contact = vJ[0];
            float maxProj = glm::dot(vJ[0], iNormal);
            for (const auto& v : vJ)
            {
                float proj = glm::dot(v, iNormal);
                if (proj > maxProj) { maxProj = proj; contact = v; }
            }
            return contact;
        }

        // CIRCLE-QUAD: use circle surface point or quad's deepest vertex
        case 3:
            if (bodyI.Model == Collider::CIRCLE) return bodyI.Position - iNormal * (bodyI.Size.x * 0.5f);
            else
            {
                // bodyI is QUAD: bodyJ is CIRCLE, contact on its surface facing bodyI
                return bodyJ.Position + iNormal * (bodyJ.Size.x * 0.5f);
            }
        }
        return (bodyI.Position + bodyJ.Position) * 0.5f;
    }

    void PhysicsSystem2D::resolveCollision(
        PhysicalBody2D& bodyI,
        PhysicalBody2D& bodyJ,
        const Vector2& iNormal,
        float overlap,
        Timestep ts)
    {
        float invMassI = (bodyI.Type != BodyType::STATIC) ? bodyI.InverseMass : 0.0f;
        float invMassJ = (bodyJ.Type != BodyType::STATIC) ? bodyJ.InverseMass : 0.0f;
        float totalInvMass = invMassI + invMassJ;
        if (totalInvMass == 0.0f) return;

        float invInertiaI = (bodyI.AllowRotation && bodyI.Inertia > 0.0f) ? 1.0f / bodyI.Inertia : 0.0f;
        float invInertiaJ = (bodyJ.AllowRotation && bodyJ.Inertia > 0.0f) ? 1.0f / bodyJ.Inertia : 0.0f;

        // Contact point and moment arms
        Vector2 contact = computeContactPoint(bodyI, bodyJ, iNormal);
        Vector2 rI = contact - bodyI.Position;
        Vector2 rJ = contact - bodyJ.Position;

        // 2D cross products: (r × n) as scalars
        float crossIN = Math::cross(rI, iNormal);
        float crossJN = Math::cross(rJ, iNormal);

        // Relative velocity at contact point (including angular contribution)
        // v_contact = v + ω × r, in 2D: ω × r = ω * perp(r) = ω * (-r.y, r.x)
        Vector2 vContactI = bodyI.Velocity + bodyI.AngularVelocity * Vector2(-rI.y, rI.x);
        Vector2 vContactJ = bodyJ.Velocity + bodyJ.AngularVelocity * Vector2(-rJ.y, rJ.x);
        Vector2 relVel = vContactI - vContactJ;
        float velAlongNormal = glm::dot(relVel, iNormal);

        // Velocity impulse — only resolve if bodies are approaching
        if (velAlongNormal < 0.0f)
        {
            // Restitution: stored ElasticityCoefficient = 1+e, so e = coef - 1
            float e = ((bodyI.ElasticityCoefficient + bodyJ.ElasticityCoefficient) * 0.5f) - 1.0f;
            e = glm::clamp(e, 0.0f, 1.0f);

            float denom = totalInvMass
                + crossIN * crossIN * invInertiaI
                + crossJN * crossJN * invInertiaJ;

            float jImpulse = -(1.0f + e) * velAlongNormal / denom;
            Vector2 impulse = jImpulse * iNormal;

            if (bodyI.Type != BodyType::STATIC)
            {
                bodyI.Velocity += invMassI * impulse;
                bodyI.AngularVelocity += invInertiaI * Math::cross(rI, impulse);
            }
            if (bodyJ.Type != BodyType::STATIC)
            {
                bodyJ.Velocity -= invMassJ * impulse;
                bodyJ.AngularVelocity -= invInertiaJ * Math::cross(rJ, impulse);
            }

            // Friction
            Vector2 tangent = relVel - velAlongNormal * iNormal;
            float tangentLen = glm::length(tangent);
            if (tangentLen > EPSILON)
            {
                tangent /= tangentLen;
                float crossIT = Math::cross(rI, tangent);
                float crossJT = Math::cross(rJ, tangent);
                float denomT = totalInvMass
                    + crossIT * crossIT * invInertiaI
                    + crossJT * crossJT * invInertiaJ;

                float jt = -glm::dot(relVel, tangent) / denomT;
                float mu = (bodyI.getFrictionCoefficient() + bodyJ.getFrictionCoefficient()) * 0.5f;
                float frictionMag = glm::clamp(jt, -mu * jImpulse, mu * jImpulse);
                Vector2 frictionImpulse = frictionMag * tangent;

                if (bodyI.Type != BodyType::STATIC)
                {
                    bodyI.Velocity += invMassI * frictionImpulse;
                    bodyI.AngularVelocity += invInertiaI * Math::cross(rI, frictionImpulse);
                }
                if (bodyJ.Type != BodyType::STATIC)
                {
                    bodyJ.Velocity -= invMassJ * frictionImpulse;
                    bodyJ.AngularVelocity -= invInertiaJ * Math::cross(rJ, frictionImpulse);
                }
            }
        }

        // Position correction — push bodies apart proportional to inverse mass
        float correctionMag = CORRECTION_FACTOR * overlap / totalInvMass;
        Vector2 correction = correctionMag * iNormal;
        if (bodyI.Type != BodyType::STATIC) bodyI.Position += invMassI * correction;
        if (bodyJ.Type != BodyType::STATIC) bodyJ.Position -= invMassJ * correction;
    }

    void PhysicsSystem2D::onUpdate(Timestep ts)
    {
        ts = std::min(1.0f / 30.0f, static_cast<float>(ts));

        std::for_each(
            std::execution::seq,
            m_Bodies.begin(), 
            m_Bodies.end(), 
            [ts, this](PhysicalBody2D& body)
            {
                if (body.Status != BodyStatus::INACTIVE)
                {
                    if (body.Type == BodyType::DYNAMIC)
                    {
                        if (Gravity) body.Force.y = GravitationalAccel * body.Mass;
                        body.onUpdate(ts);

                        if (body.AllowRotation) body.Rotation += body.AngularVelocity * static_cast<float>(ts);
                    }
                    else
                    {
                        body.Acceleration = { 0.0f, 0.0f };
                        body.Velocity = { 0.0f, 0.0f };

                        body.Force = { 0.0f, 0.0f };
                    }

                    body.Impulse = { 0.0f, 0.0f };
                    body.ContactImpulse = { 0.0f, 0.0f };
                    body.ContactNormal = { 0.0f, 0.0f };
                    body.CallbackExecutions = 0;

                    body.updateBoundsAndNormals();
                }
            }
        );

        size_t index = 0;
        std::vector<BodyHandle> ssBodies(m_Bodies.size() - m_InactiveBodies.size());
        for (uint32_t i = 0; i < m_Bodies.size(); ++i)
        {
            if (m_Bodies[i].Status == BodyStatus::INACTIVE) continue;
            ssBodies[index++] = i;
        }
        std::sort(
            ssBodies.begin(),
            ssBodies.end(),
            [this](BodyHandle i, BodyHandle j)
            {
                return (m_Bodies[i].MinimumVertex.y < m_Bodies[j].MinimumVertex.y);
            }
        );

        for (size_t a = 0; a < ssBodies.size(); ++a)
        {
            BodyHandle i = ssBodies[a];
            for (size_t b = a + 1; b < ssBodies.size(); ++b)
            {
                BodyHandle j = ssBodies[b];
                if (m_Bodies[j].MinimumVertex.y > m_Bodies[i].MaximumVertex.y) break;

                if (m_Bodies[j].Type == BodyType::STATIC && m_Bodies[i].Type == BodyType::STATIC) continue;

                if (checkBroadPhase(m_Bodies[i], m_Bodies[j]))
                {
                    CollisionInfo info;
                    checkNarrowPhase(m_Bodies[i], m_Bodies[j], info);
                    if (info.Collision)
                    {
                        resolveCollision(m_Bodies[i], m_Bodies[j], info.CollisionInfoPair[0].Normal, info.minOverlap, ts);
                        if (m_Bodies[i].Callback && m_Bodies[i].CallbackExecutions < m_Bodies[i].NumberOfExecution)
                        {
                            m_Bodies[i].Callback(
                                m_Bodies[i],
                                m_Bodies[j],
                                { true, info.minOverlap, { info.CollisionInfoPair[0], info.CollisionInfoPair[1] }, ts }
                            );
                        }

                        if (m_Bodies[j].Callback && m_Bodies[j].CallbackExecutions < m_Bodies[j].NumberOfExecution)
                        {
                            m_Bodies[j].Callback(
                                m_Bodies[j],
                                m_Bodies[i],
                                { true, info.minOverlap, { info.CollisionInfoPair[1], info.CollisionInfoPair[0] }, ts }
                            );
                        }
                    }
                }
            }
        }
    }

    void PhysicsSystem2D::Update()
    {
        while (!m_Shutdown.load(std::memory_order_relaxed))
        {
            auto frameID = Application::Get().getFrameID();
            auto timestep = Application::Get().getTimestep();
            if (m_LastFrameID < frameID && m_RunUpdateThread.load(std::memory_order_acquire))
            {
                m_LastFrameID = frameID;
                std::scoped_lock lock(m_UpdateMutex);
                onUpdate(timestep);
            }
        }
    }
}