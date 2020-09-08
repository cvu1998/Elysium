#include "PhysicsSystem.h"

#include <algorithm>

#define NOMINMAX
#include "Elysium/Log.h"
#include "Elysium/Math.h"

namespace Elysium
{
    PhysicsSystem::PhysicsSystem(float acceleration) : 
        m_GravitationalAccel(acceleration)
    {
    }

    PhysicsSystem::~PhysicsSystem()
    {
    }

    PhysicalBody* PhysicsSystem::createPhysicalBody(BodyType type, const char* name, float mass, const Vector2& initialPosition, const Vector2& size, PhysicalBody::Collision_Callback callback)
    {
        if (m_InactiveBodies.empty())
        {
            m_Bodies.push_back(PhysicalBody::createPhysicalBody(type, name, mass, initialPosition, size, callback));
            return &m_Bodies[m_Bodies.size() - 1];
        }
        BodyHandle body = m_InactiveBodies.back();
        m_InactiveBodies.pop_back();
        m_Bodies[body] = PhysicalBody(type, name, mass, initialPosition, size, callback);
        return &m_Bodies[body];
    }

    void PhysicsSystem::createPhysicalBody(BodyHandle* handle, BodyType type, const char* name, float mass, const Vector2& initialPosition, const Vector2& size,
        PhysicalBody::Collision_Callback callback)
    {
        if (m_InactiveBodies.empty())
        {
            m_Bodies.push_back(PhysicalBody::createPhysicalBody(type, name, mass, initialPosition, size, callback));
            *handle = (BodyHandle)m_Bodies.size() - 1;
            return;
        }
        BodyHandle body = m_InactiveBodies.back();
        m_InactiveBodies.pop_back();
        m_Bodies[body] = PhysicalBody(type, name, mass, initialPosition, size, callback);
        *handle = body;
    }

    void PhysicsSystem::updateBody(PhysicalBody& body, Timestep ts)
    {
        for (auto& impulse : body.Normals)
        {
            body.ContactNormal += impulse.first;
            body.Impulse += impulse.first * impulse.second;
        }
        body.Normals.clear();

        body.Acceleration = body.Force / body.Mass;
        body.Velocity = body.Velocity + (body.Impulse / body.Mass) + (body.Acceleration * (float)ts);
        body.Position = body.Position + (body.Velocity * (float)ts);

        body.Force = { 0.0f, 0.0f };
        if (Gravity)
            body.Force.y = -m_GravitationalAccel * body.Mass;
    }

    void PhysicsSystem::applyCollisionResponse(PhysicalBody& body, const PhysicalBody& otherBody, const Vector2& normal, Timestep ts)
    {
        /***** Normal Force *****/
        body.Force += abs(body.Force) * normal;

        Vector2 impulse = { 0.0f, 0.0f };

        if (body.Type == BodyType::DYNAMIC)
            impulse = abs(otherBody.Velocity / body.Mass);

        float averageElasticity = (body.ElasticityCoefficient + otherBody.ElasticityCoefficient) * 0.5f;
        impulse += abs(body.Velocity) * body.Mass * (averageElasticity);

        body.Normals[normal] = max(impulse, body.Normals[normal]);

        /***** Friction *****/
        Vector2 frictionDirection = { 0.0f, 0.0f };
        if (body.Velocity.x * body.Velocity.x + body.Velocity.y * body.Velocity.y > 0.0f)
        {
            frictionDirection = { fabs(normal.y),  fabs(normal.x) };
            frictionDirection = -(frictionDirection * normalize(body.Velocity));
            body.Impulse += abs(body.Velocity) * frictionDirection * body.Mass * body.getFrictionCoefficient() * (float)(ts * 10.0f);
        }
    }

    bool PhysicsSystem::checkBroadPhase(const PhysicalBody& body1, const PhysicalBody& body2)
    {
        Vector2 min1 = body1.MinimumVertex;
        Vector2 max1 = body1.MaximumVertex;
        Vector2 min2 = body2.MinimumVertex;
        Vector2 max2 = body2.MaximumVertex;

        bool CollisionX = max1.x >= min2.x && min1.x <= max2.x;
        bool CollisionY = max1.y >= min2.y && min1.y <= max2.y;
        return CollisionX && CollisionY;
    }

    void PhysicsSystem::checkNarrowPhase(const PhysicalBody& body1, const PhysicalBody& body2, CollisionInfo& info)
    {
        if (body1.Radius > 0.0f && body2.Radius > 0.0f)
        {
            float distance = (body2.Position.x - body1.Position.x) * (body2.Position.x - body1.Position.x) +
                (body2.Position.y - body1.Position.y) * (body2.Position.y - body1.Position.y);
            if (distance > (body2.Radius + body1.Radius) * (body2.Radius + body1.Radius))
            {
                info.Collision = false;
                return;
            }
            Vector2 normal = normalize(body2.Position - body1.Position);
            info.CollisionInfoPair.second.Normal = normal;
            info.CollisionInfoPair.first.Normal = -normal;

            float min1 = dot(body1.Position - (normal * body1.Radius), normal);
            float max1 = dot(body1.Position + (normal * body1.Radius), normal);
            float min2 = dot(body2.Position - (normal * body2.Radius), normal);
            float max2 = dot(body2.Position + (normal * body2.Radius), normal);
            float overlap1 = fabs(std::min(max1, max2) - std::max(min1, min2));

            info.minOverlap = std::min(max1, max2) - std::max(min1, min2);
            return;
        }
        std::vector<Vector2>& vertices1 = body1.getVertices();
        std::vector<Vector2>& vertices2 = body2.getVertices();
        std::vector<Vector2> normals;
        for (Vector2& normal : body1.getNormals())
            normals.push_back(normalize(normal));
        for (Vector2& normal : body2.getNormals())
            normals.push_back(normalize(normal));

        float overlap = 0.0f;
        Vector2 collisionNormal = { 0.0f, 0.0f };
        bool firstIsMax = false;
        for (Vector2& normal : normals)
        {
            float min1 = std::numeric_limits<float>::max();
            float max1 = -std::numeric_limits<float>::max();
            if (body1.Radius > 0.0f)
            {
                Vector2 circleVertex = normal * body1.Radius;
                min1 = dot(body1.Position - (circleVertex), normal);
                max1 = dot(body1.Position + (circleVertex), normal);
            }
            else
            {
                for (Vector2& vertex : vertices1)
                {
                    float projection = dot(vertex, normal);
                    min1 = std::min(min1, projection);
                    max1 = std::max(max1, projection);
                }
            }

            float min2 = std::numeric_limits<float>::max();
            float max2 = -std::numeric_limits<float>::max();
            if (body2.Radius > 0.0f)
            {
                Vector2 circleVertex = normal * body2.Radius;
                min2 = dot(body2.Position - (circleVertex), normal);
                max2 = dot(body2.Position + (circleVertex), normal);
            }
            else
            {
                for (Vector2& vertex : vertices2)
                {
                    float projection = dot(vertex, normal);
                    min2 = std::min(min2, projection);
                    max2 = std::max(max2, projection);
                }
            }

            if (!(max2 >= min1 && max1 >= min2))
            {
                info.Collision = false;
                return;
            }

            overlap = std::min(max1, max2) - std::max(min1, min2);
            if (overlap < info.minOverlap)
            {
                collisionNormal = normal;
                info.minOverlap = overlap;
                if (max2 > max1)
                {
                    firstIsMax = false;
                }
                else
                {
                    firstIsMax = true;
                }
            }
        }

        if (firstIsMax)
        {
            info.CollisionInfoPair.first.Normal = collisionNormal;
            info.CollisionInfoPair.second.Normal = collisionNormal * -1.0f;
        }
        else
        {
            info.CollisionInfoPair.second.Normal = collisionNormal;
            info.CollisionInfoPair.first.Normal = collisionNormal * -1.0f;
        }
    }

    void PhysicsSystem::removePhysicalBody(BodyHandle body)
    {
        m_Bodies[body].Status = BodyStatus::INACTIVE;
        m_InactiveBodies.push_back(body);
        std::sort(m_InactiveBodies.begin(), m_InactiveBodies.end(), std::greater());
    }

    void PhysicsSystem::onUpdate(Timestep ts)
    {
        m_Time += ts;
        for (uint32_t i = 0; i < m_Bodies.size(); i++)
        {
            if (m_Bodies[i].Status != BodyStatus::INACTIVE)
            {
                switch (m_Bodies[i].Type)
                {
                case BodyType::DYNAMIC:
                    updateBody(m_Bodies[i], ts);
                    if (m_Bodies[i].Radius > 0.0f)
                        m_Bodies[i].ContactNormal = (isNaN(normalize(m_Bodies[i].ContactNormal)) || isinf(normalize(m_Bodies[i].ContactNormal))) ?
                            m_Bodies[i].ContactNormal : normalize(m_Bodies[i].ContactNormal);
                    else
                        m_Bodies[i].ContactNormal = (isNaN(normalize(m_Bodies[i].Impulse)) || isinf(normalize(m_Bodies[i].Impulse))) ?
                        m_Bodies[i].ContactNormal : normalize(m_Bodies[i].Impulse);

                    if (m_Bodies[i].AllowRotation)
                        m_Bodies[i].Rotation += (cross(m_Bodies[i].ContactNormal, m_Bodies[i].Velocity) / m_Bodies[i].Inertia) * (float)ts;
                    break;
                case BodyType::KINEMATIC:
                    updateBody(m_Bodies[i], ts);
                    break;
                default:
                    m_Bodies[i].Acceleration = { 0.0f, 0.0f };
                    m_Bodies[i].Velocity = { 0.0f, 0.0f };

                    m_Bodies[i].Force = { 0.0f, 0.0f };
                }
                m_Bodies[i].Impulse = { 0.0f, 0.0f };
                m_Bodies[i].CallbackExecutions = 0;

                m_Bodies[i].MaximumVertex = m_Bodies[i].getMaxVertex();
                m_Bodies[i].MinimumVertex = m_Bodies[i].getMinVertex();
            }
        }

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

                        float magnitude1 = fabs(dot(m_Bodies[i].Velocity, iNormal));
                        float magnitude2 = fabs(dot(m_Bodies[j].Velocity, jNormal));
                        if (magnitude1 > magnitude2)
                            m_Bodies[i].Position = m_Bodies[i].Position + (info.minOverlap * iNormal);
                        else
                             m_Bodies[j].Position = m_Bodies[j].Position + (info.minOverlap * jNormal);

                        if (m_Bodies[i].Type != BodyType::STATIC)
                            applyCollisionResponse(m_Bodies[i], m_Bodies[j], iNormal, ts);

                        if (m_Bodies[j].Type != BodyType::STATIC)
                            applyCollisionResponse(m_Bodies[j], m_Bodies[i], jNormal, ts);
#ifdef LOG
                        ELY_CORE_TRACE("NarrowPhaseCollision: {0}, {1}", m_Bodies[i].Name, m_Bodies[j].Name);
                        ELY_CORE_TRACE("CollisionInfo Overlap: {0}", info.minOverlap);
                        ELY_CORE_TRACE("CollisionInfo Normal: {0}: {1}, {2}", m_Bodies[i].Name, iNormal.x, iNormal.y);
                        ELY_CORE_TRACE("CollisionInfo Normal: {0}: {1}, {2}", m_Bodies[j].Name, jNormal.x, jNormal.y);
                        ELY_CORE_TRACE("CollisionInfo Velocity: {0}: {1}, {2}", m_Bodies[i].Name, m_Bodies[i].Velocity.x, m_Bodies[i].Velocity.y);
                        ELY_CORE_TRACE("CollisionInfo Velocity: {0}: {1}, {2}", m_Bodies[j].Name, m_Bodies[j].Velocity.x, m_Bodies[j].Velocity.y);
                        ELY_CORE_TRACE("CollisionInfo Force: {0}: {1}, {2}", m_Bodies[i].Name, m_Bodies[i].Force.x, m_Bodies[i].Force.y);
                        ELY_CORE_TRACE("CollisionInfo Force: {0}: {1}, {2}", m_Bodies[j].Name, m_Bodies[j].Force.x, m_Bodies[j].Force.y);
                        ELY_CORE_TRACE("CollisionInfo Impulse: {0}: {1}, {2}", m_Bodies[i].Name, m_Bodies[i].Impulse.x, m_Bodies[i].Impulse.y);
                        ELY_CORE_TRACE("CollisionInfo Impulse: {0}: {1}, {2}", m_Bodies[j].Name, m_Bodies[j].Impulse.x, m_Bodies[j].Impulse.y);
#endif
                        if (m_Bodies[i].Callback && m_Bodies[i].CallbackExecutions < m_Bodies[i].NumberOfExecution)
                            m_Bodies[i].Callback(m_Bodies[i], m_Bodies[j], { true, info.minOverlap, ts,
                                std::make_pair(info.CollisionInfoPair.first, info.CollisionInfoPair.second) });

                        if (m_Bodies[j].Callback && m_Bodies[j].CallbackExecutions < m_Bodies[j].NumberOfExecution)
                            m_Bodies[j].Callback(m_Bodies[j], m_Bodies[i],{ true, info.minOverlap, ts,
                                    std::make_pair(info.CollisionInfoPair.second, info.CollisionInfoPair.first) });
                    }
                }
            }
        }
    }
}