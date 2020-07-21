#include "PhysicsSystem.h"

#include <algorithm>

#include "Elysium/Math.h"

namespace Elysium
{
    PhysicsSystem::PhysicsSystem(float acceleration) : 
        m_GravitationalAccel(acceleration)
#ifdef LOG
        , m_LogFile("Physics-Log.txt")
#endif
    {
    }

    PhysicsSystem::~PhysicsSystem()
    {
#ifdef LOG
        m_LogFile.close();
#endif
    }

    PhysicsBody PhysicsSystem::createPhysicalBody(BodyType type, const char* name, float mass, const Vector2& initialPosition, const Vector2& size, 
        PhysicalBody::Collision_Callback callback)
    {
        if (m_InactiveBodies.empty())
        {
            m_Bodies.emplace_back(type, name, mass, initialPosition, size, callback);
            return (PhysicsBody)m_Bodies.size() - 1;
        }
        PhysicsBody body = m_InactiveBodies.back();
        m_InactiveBodies.pop_back();
        m_Bodies[body] = PhysicalBody(type, name, mass, initialPosition, size, callback);
        return body;
    }

    bool PhysicsSystem::checkBroadPhase(const PhysicalBody& body1, const PhysicalBody& body2)
    {
        Vector2 min1 = body1.Position - body1.BroadSize * 0.5f;
        Vector2 max1 = body1.Position + body1.BroadSize * 0.5f;
        Vector2 min2 = body2.Position - body2.BroadSize * 0.5f;
        Vector2 max2 = body2.Position + body2.BroadSize * 0.5f;

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

    void PhysicsSystem::removePhysicalBody(PhysicsBody body)
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
                    m_Bodies[i].Acceleration = m_Bodies[i].Force / m_Bodies[i].Mass;
                    m_Bodies[i].Velocity = m_Bodies[i].Velocity + (m_Bodies[i].Impulse / m_Bodies[i].Mass) + (m_Bodies[i].Acceleration * (float)ts);
                    m_Bodies[i].Position = m_Bodies[i].Position + (m_Bodies[i].Velocity * (float)ts);

                    m_Bodies[i].Force = { 0.0f, 0.0f };
                    m_Bodies[i].Impulse = { 0.0f, 0.0f };

                    if (Gravity && m_Bodies[i].Status != BodyStatus::NOGRAVITY)
                    {
                        m_Bodies[i].Force.y = -m_GravitationalAccel * m_Bodies[i].Mass;
                    }
                    break;
                default:
                    m_Bodies[i].Acceleration = { 0.0f, 0.0f };
                    m_Bodies[i].Velocity = { 0.0f, 0.0f };
                    
                    m_Bodies[i].Force = { 0.0f, 0.0f };
                    m_Bodies[i].Impulse = { 0.0f, 0.0f };
                }
            }
        }

        for (uint32_t i = 0; i < m_Bodies.size(); i++)
        {
            if (m_Bodies[i].Status != BodyStatus::INACTIVE)
            {
                for (uint32_t j = i + 1; j < m_Bodies.size(); j++)
                {
                    if (m_Bodies[j].Status != BodyStatus::INACTIVE)
                    {
                        if (m_Bodies[i].Type == BodyType::STATIC && m_Bodies[j].Type == BodyType::STATIC)
                            continue;

                        if (checkBroadPhase(m_Bodies[i], m_Bodies[j]))
                        {
                            CollisionInfo info;
                            checkNarrowPhase(m_Bodies[i], m_Bodies[j], info);

                            if (info.Collision)
                            {
                                Vector2 ObjectINormal = info.CollisionInfoPair.first.Normal;
                                Vector2 ObjectJNormal = info.CollisionInfoPair.second.Normal;

                                float magnitude1 = fabs(dot(m_Bodies[i].Velocity, ObjectINormal));
                                float magnitude2 = fabs(dot(m_Bodies[j].Velocity, ObjectJNormal));
                                if (magnitude1 > magnitude2)
                                {
                                    if (m_Bodies[i].Type == BodyType::DYNAMIC)
                                        m_Bodies[i].Position = m_Bodies[i].Position + (info.minOverlap * ObjectINormal);
                                    else
                                        m_Bodies[j].Position = m_Bodies[j].Position + (info.minOverlap * ObjectJNormal);
                                }
                                else
                                {
                                    if (m_Bodies[j].Type == BodyType::DYNAMIC)
                                        m_Bodies[j].Position = m_Bodies[j].Position + (info.minOverlap * ObjectJNormal);
                                    else
                                        m_Bodies[i].Position = m_Bodies[i].Position + (info.minOverlap * ObjectINormal);
                                }
                                /***** Normal Force *****/
                                m_Bodies[i].Force += abs(m_Bodies[i].Force) * ObjectINormal;
                                m_Bodies[j].Force += abs(m_Bodies[j].Force) * ObjectJNormal;

                                /***** Conservation of Momentum *****/
                                m_Bodies[i].Impulse += abs(m_Bodies[j].Velocity) * ObjectINormal;
                                m_Bodies[j].Impulse += abs(m_Bodies[i].Velocity) * ObjectJNormal;

                                /***** Normal Collision Impulse Response *****/
                                float averageElasticity = (m_Bodies[i].ElasticityCoefficient + m_Bodies[j].ElasticityCoefficient) * 0.5f;
                                m_Bodies[i].Impulse += abs(m_Bodies[i].Velocity) * ObjectINormal * m_Bodies[i].Mass * (averageElasticity);
                                m_Bodies[j].Impulse += abs(m_Bodies[j].Velocity) * ObjectJNormal * m_Bodies[j].Mass * (averageElasticity);

                                /***** Friction *****/
                                Vector2 frictionDirection = { 0.0f, 0.0f };
                                if (m_Bodies[i].Velocity.x * m_Bodies[i].Velocity.x + m_Bodies[i].Velocity.y * m_Bodies[i].Velocity.y > 0.0f)
                                {
                                    frictionDirection = { fabs(ObjectINormal.y),  fabs(ObjectINormal.x) };
                                    frictionDirection = -(frictionDirection * normalize(m_Bodies[i].Velocity));
                                    m_Bodies[i].Impulse += abs(m_Bodies[i].Velocity) * frictionDirection * m_Bodies[i].Mass * m_Bodies[j].getFrictionCoefficient() * (float)(ts * 10.0f);
                                }
                                if (m_Bodies[j].Velocity.x * m_Bodies[j].Velocity.x + m_Bodies[j].Velocity.y * m_Bodies[j].Velocity.y > 0.0f)
                                {
                                    frictionDirection = { fabs(ObjectJNormal.y) , fabs(ObjectJNormal.x) };
                                    frictionDirection = -(frictionDirection * normalize(m_Bodies[j].Velocity));
                                    m_Bodies[j].Impulse += abs(m_Bodies[j].Velocity) * frictionDirection * m_Bodies[j].Mass * m_Bodies[i].getFrictionCoefficient() * (float)(ts * 10.0f);
                                }
#ifdef LOG
                                m_LogFile << m_Time << ": NarrowPhaseCollision: " << m_Bodies[i].Name << ", " << m_Bodies[j].Name << std::endl;
                                m_LogFile << m_Time << ": CollisionInfo Overlap: " << info.minOverlap << std::endl;
                                m_LogFile << m_Time << ": CollisionInfo Normal: " << m_Bodies[i].Name << ": " << ObjectINormal.x << ", " << ObjectINormal.y << std::endl;
                                m_LogFile << m_Time << ": CollisionInfo Normal: " << m_Bodies[j].Name << ": " << ObjectJNormal.x << ", " << ObjectJNormal.y << std::endl;
                                m_LogFile << m_Time << ": CollisionInfo Velocity: " << m_Bodies[i].Name << ": " << m_Bodies[i].Velocity.x << ", " << m_Bodies[i].Velocity.y << std::endl;
                                m_LogFile << m_Time << ": CollisionInfo Velocity: " << m_Bodies[j].Name << ": " << m_Bodies[j].Velocity.x << ", " << m_Bodies[j].Velocity.y << std::endl;
                                m_LogFile << m_Time << ": CollisionInfo Force: " << m_Bodies[i].Name << ": " << m_Bodies[i].Force.x << ", " << m_Bodies[i].Force.y << std::endl;
                                m_LogFile << m_Time << ": CollisionInfo Force: " << m_Bodies[j].Name << ": " << m_Bodies[j].Force.x << ", " << m_Bodies[j].Force.y << std::endl;
                                m_LogFile << m_Time << ": CollisionInfo Impulse: " << m_Bodies[i].Name << ": " << m_Bodies[i].Impulse.x << ", " << m_Bodies[i].Impulse.y << std::endl;
                                m_LogFile << m_Time << ": CollisionInfo Impulse: " << m_Bodies[j].Name << ": " << m_Bodies[j].Impulse.x << ", " << m_Bodies[j].Impulse.y << std::endl;
#endif
                                if (m_Bodies[i].Callback)
                                    m_Bodies[i].Callback(m_Bodies[i], { true, info.minOverlap, ts,
                                        std::make_pair(info.CollisionInfoPair.first, info.CollisionInfoPair.second) });

                                if (m_Bodies[j].Callback)
                                    m_Bodies[j].Callback(m_Bodies[i], { true, info.minOverlap, ts,
                                         std::make_pair(info.CollisionInfoPair.second, info.CollisionInfoPair.first) });
                            }
                        }
                    }
                }
            }
        }
    }
}