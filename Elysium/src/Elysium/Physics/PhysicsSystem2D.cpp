#include "PhysicsSystem2D.h"

#include <algorithm>
#include <execution>
#include <string.h>

#define NOMINMAX
#include "Elysium/Log.h"
#include "Elysium/MathUtility.h"

namespace Elysium
{
    PhysicalBody2D* PhysicsSystem2D::createPhysicalBody(BodyType type, Collider collider, const char* name, float mass,
        const Vector2& initialPosition, const Vector2& size, 
        PhysicalBody2D::Collision_Callback callback)
    {
        if (m_InactiveBodies.empty())
        {
            m_Bodies.push_back(std::move(PhysicalBody2D(type, collider, name, mass, initialPosition, size, callback)));
            return &m_Bodies[m_Bodies.size() - 1];
        }
        BodyHandle body = m_InactiveBodies.back();
        m_InactiveBodies.pop_back();
        m_Bodies[body] = PhysicalBody2D(type, collider, name, mass, initialPosition, size, callback);
        return &m_Bodies[body];
    }

    void PhysicsSystem2D::createPhysicalBody(BodyHandle* handle, BodyType type, Collider collider, const char* name, float mass,
        const Vector2& initialPosition, const Vector2& size,
        PhysicalBody2D::Collision_Callback callback)
    {
        if (m_InactiveBodies.empty())
        {
            m_Bodies.push_back(std::move(PhysicalBody2D(type, collider, name, mass, initialPosition, size, callback)));
            *handle = (BodyHandle)m_Bodies.size() - 1;
            return;
        }
        BodyHandle body = m_InactiveBodies.back();
        m_InactiveBodies.pop_back();
        m_Bodies[body] = PhysicalBody2D(type, collider, name, mass, initialPosition, size, callback);
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
        std::vector<Vector2> normals;

        int result = (int)body1.Model | (int)body2.Model;
        switch (result)
        {
        // CIRCLE-CIRCLE COLLISION
        case 2:
        {
            float radius1 = body1.Size.x * 0.5f;
            float radius2 = body2.Size.x * 0.5f;
            float distance = (body2.Position.x - body1.Position.x) * (body2.Position.x - body1.Position.x) +
                (body2.Position.y - body1.Position.y) * (body2.Position.y - body1.Position.y);
            if (distance < (radius2 + radius1) * (radius2 + radius1))
            {
                info.Collision = true;

                Vector2 normal = glm::normalize(body2.Position - body1.Position);
                info.CollisionInfoPair.second.Normal = normal;
                info.CollisionInfoPair.first.Normal = -normal;

                float min1 = glm::dot(body1.Position - (normal * radius1), normal);
                float max1 = glm::dot(body1.Position + (normal * radius1), normal);
                float min2 = glm::dot(body2.Position - (normal * radius2), normal);
                float max2 = glm::dot(body2.Position + (normal * radius2), normal);
                float overlap1 = fabs(std::min(max1, max2) - std::max(min1, min2));

                info.minOverlap = glm::min(max1, max2) - glm::max(min1, min2);
            }
        }
            break;
        // QUAD-QUAD COLLISION
        case 1:
        {
            std::vector<Vector2> vertices1;
            std::vector<Vector2> vertices2;
            body1.getVertices(vertices1);
            body2.getVertices(vertices2);

            for (const Vector2& normal : body1.Normals)
                normals.push_back(normal);
            for (const Vector2& normal : body2.Normals)
                normals.push_back(normal);

            float overlap = 0.0f;
            Vector2 collisionNormal = { 0.0f, 0.0f };
            bool firstIsMax = false;
            for (Vector2& normal : normals)
            {
                float min1 = std::numeric_limits<float>::max();
                float max1 = -std::numeric_limits<float>::max();
                for (Vector2& vertex : vertices1)
                {
                    float projection = dot(vertex, normal);
                    min1 = std::min(min1, projection);
                    max1 = std::max(max1, projection);
                }

                float min2 = std::numeric_limits<float>::max();
                float max2 = -std::numeric_limits<float>::max();
                for (Vector2& vertex : vertices2)
                {
                    float projection = dot(vertex, normal);
                    min2 = std::min(min2, projection);
                    max2 = std::max(max2, projection);
                }

                if (!(max2 >= min1 && max1 >= min2))
                    return;

                overlap = std::min(max1, max2) - std::max(min1, min2);
                if (overlap < info.minOverlap)
                {
                    collisionNormal = normal;
                    info.minOverlap = overlap;
                    firstIsMax = max1 > max2;
                }
            }

            info.Collision = true;
            info.CollisionInfoPair.first.Normal = -collisionNormal;
            info.CollisionInfoPair.second.Normal = collisionNormal;
            if (firstIsMax)
            {
                info.CollisionInfoPair.first.Normal = collisionNormal;
                info.CollisionInfoPair.second.Normal = -collisionNormal;
            }
        }
            break;
        // CIRCLE-QUAD COLLISION
        case 3:
        {
            bool circleFirst = true;
            PhysicalBody2D circle = body1;
            PhysicalBody2D quad = body2;
            if (body2.Model == Collider::CIRCLE)
            {
                circleFirst = false;
                circle = body2;
                quad = body1;
            }

            std::vector<Vector2> vertices;
            quad.getVertices(vertices);

            for (const Vector2& normal : quad.Normals)
                normals.push_back(normal);

            float overlap = 0.0f;
            Vector2 collisionNormal = { 0.0f, 0.0f };
            bool firstIsMax = false;
            for (Vector2& normal : normals)
            {
                Vector2 circleVertex = normal * circle.Size.x * 0.5f;
                float min1 = dot(circle.Position - (circleVertex), normal);
                float max1 = dot(circle.Position + (circleVertex), normal);

                float min2 = std::numeric_limits<float>::max();
                float max2 = -std::numeric_limits<float>::max();
                for (Vector2& vertex : vertices)
                {
                    float projection = dot(vertex, normal);
                    min2 = std::min(min2, projection);
                    max2 = std::max(max2, projection);
                }

                if (!(max2 >= min1 && max1 >= min2))
                    return;

                overlap = std::min(max1, max2) - std::max(min1, min2);
                if (overlap < info.minOverlap)
                {
                    collisionNormal = normal;
                    info.minOverlap = overlap;
                    firstIsMax = max1 > max2;
                }
            }

            info.Collision = true;
            info.CollisionInfoPair.second.Normal = collisionNormal;
            info.CollisionInfoPair.first.Normal = -collisionNormal;
            if (firstIsMax && circleFirst || !firstIsMax && !circleFirst)
            {
                info.CollisionInfoPair.first.Normal = collisionNormal;
                info.CollisionInfoPair.second.Normal = -collisionNormal;
            }
        }
            break;
        }
    }

    void PhysicsSystem2D::logInfo(const char* tag)
    {
        for (uint32_t i = 0; i < m_Bodies.size(); i++)
        {
            if (m_Bodies[i].Status != BodyStatus::INACTIVE && strcmp(m_Bodies[i].Tag, tag) == 0)
            {
                m_LoggedBodies.insert(i);
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
        ELY_CORE_TRACE("-----Body Tag: {0}-----", m_Bodies[i].Tag);
    }

    void PhysicsSystem2D::applyCollisionResponse(PhysicalBody2D& body, const PhysicalBody2D& otherBody, const Vector2& normal, float overlap,
        Timestep ts)
    {
        /***** Normal Force *****/
        body.Force += abs(body.Force) * normal;

        float averageElasticity = (body.ElasticityCoefficient + otherBody.ElasticityCoefficient) * 0.5f;
        Vector2 impulse = abs(body.Velocity) * body.Mass * (averageElasticity);

        if (body.Type == BodyType::DYNAMIC)
            impulse += abs(otherBody.Velocity * otherBody.Mass / body.Mass) * (float)ts;

        body.ContactImpulse.x = glm::max(impulse.x, body.ContactImpulse.x);
        body.ContactImpulse.y = glm::max(impulse.y, body.ContactImpulse.y);

        body.ContactNormal += normal;

        /***** Friction *****/
        Vector2 frictionDirection = { 0.0f, 0.0f };
        if (body.Velocity.x * body.Velocity.x + body.Velocity.y * body.Velocity.y > 0.0f)
        {
            frictionDirection = { fabs(normal.y),  fabs(normal.x) };
            frictionDirection = -(frictionDirection * normalize(body.Velocity));
            body.Impulse += abs(body.Velocity) * body.Mass * glm::dot(frictionDirection, 2.0f * body.Size * body.Size) * body.getFrictionCoefficient() * (float)(ts);
        }
    }

    void PhysicsSystem2D::onUpdate(Timestep ts)
    {
        ts = std::min(1.0f / 30.0f, (float)ts);

        std::for_each(std::execution::par_unseq, 
            m_Bodies.begin(), 
            m_Bodies.end(), 
            [&](PhysicalBody2D& body)
            {
                if (body.Status != BodyStatus::INACTIVE)
                {
                    switch (body.Type)
                    {
                    case BodyType::DYNAMIC:
                        body.onUpdate(ts);
                        if (Gravity)
                            body.Force.y = GravitationalAccel * body.Mass;

                        if (body.AllowRotation)
                        {
                            switch (body.Model)
                            {
                            case Collider::CIRCLE:
                                body.AngularVelocity = Math::cross(body.Normal, body.Velocity) / body.Inertia;
                                body.Rotation += body.AngularVelocity * (float)ts;
                                break;
                            case Collider::QUAD:
                            {
                            }
                            break;
                            }
                        }
                        break;
                    case BodyType::KINEMATIC:
                        body.onUpdate(ts);
                        if (Gravity)
                            body.Force.y = GravitationalAccel * body.Mass;

                        break;
                    case BodyType::STATIC:
                        body.Acceleration = { 0.0f, 0.0f };
                        body.Velocity = { 0.0f, 0.0f };

                        body.Force = { 0.0f, 0.0f };
                    }

#ifdef __DEBUG
                    if (m_LoggedBodies.find(i) != m_LoggedBodies.end())
                        printInfo(i);
#endif

                    body.Impulse = { 0.0f, 0.0f };
                    body.ContactImpulse = { 0.0f, 0.0f };
                    body.ContactNormal = { 0.0f, 0.0f };
                    body.CallbackExecutions = 0;

                    body.MaximumVertex = body.getMaxVertex();
                    body.MinimumVertex = body.getMinVertex();
                    body.updateNormals();
                }
            });

        size_t index = 0;
        std::vector<BodyHandle> ssBodies(m_Bodies.size() - m_InactiveBodies.size());
        for (uint32_t i = 0; i < m_Bodies.size(); i++)
        {
            if (m_Bodies[i].Status != BodyStatus::INACTIVE)
            {
                ssBodies[index] = i;
                index++;
            }

        }
        std::sort(ssBodies.begin(), ssBodies.end(), [this](BodyHandle i, BodyHandle j)
            {
                return (m_Bodies[i].MinimumVertex.y <
                m_Bodies[j].MinimumVertex.y);
            }
        );

        for (size_t a = 0; a < ssBodies.size(); a++)
        {
            BodyHandle i = ssBodies[a];
            for (size_t b = a + 1; b < ssBodies.size(); b++)
            {
                BodyHandle j = ssBodies[b];
                if (m_Bodies[j].MinimumVertex.y >
                    m_Bodies[i].MaximumVertex.y)
                {
                    break;
                }

                if (m_Bodies[j].Type == BodyType::STATIC && m_Bodies[i].Type == BodyType::STATIC)
                    continue;

                if (checkBroadPhase(m_Bodies[i], m_Bodies[j]))
                {
                    CollisionInfo info;
                    checkNarrowPhase(m_Bodies[i], m_Bodies[j], info);
                    if (info.Collision)
                    {
                        Vector2 iNormal = info.CollisionInfoPair.first.Normal;
                        Vector2 jNormal = info.CollisionInfoPair.second.Normal;

                        float magnitude1 = glm::length(m_Bodies[i].Velocity);
                        float magnitude2 = glm::length(m_Bodies[j].Velocity);
                        if (magnitude1 > magnitude2)
                            m_Bodies[i].Position = m_Bodies[i].Position + (info.minOverlap * iNormal);
                        if (magnitude2 > magnitude1)
                             m_Bodies[j].Position = m_Bodies[j].Position + (info.minOverlap * jNormal);

                        if (m_Bodies[i].Type != BodyType::STATIC)
                            applyCollisionResponse(m_Bodies[i], m_Bodies[j], iNormal, info.minOverlap, ts);

                        if (m_Bodies[j].Type != BodyType::STATIC)
                            applyCollisionResponse(m_Bodies[j], m_Bodies[i], jNormal, info.minOverlap, ts);

                        if (m_Bodies[i].Callback && m_Bodies[i].CallbackExecutions < m_Bodies[i].NumberOfExecution)
                            m_Bodies[i].Callback(m_Bodies[i], m_Bodies[j], { true, info.minOverlap,
                                std::make_pair(info.CollisionInfoPair.first, info.CollisionInfoPair.second),
                                ts });

                        if (m_Bodies[j].Callback && m_Bodies[j].CallbackExecutions < m_Bodies[j].NumberOfExecution)
                            m_Bodies[j].Callback(m_Bodies[j], m_Bodies[i],{ true, info.minOverlap,
                                    std::make_pair(info.CollisionInfoPair.second, info.CollisionInfoPair.first),
                                ts });
                    }
                }
            }
        }
    }
}