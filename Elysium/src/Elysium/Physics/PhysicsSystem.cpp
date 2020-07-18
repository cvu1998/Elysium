#include "PhysicsSystem.h"

#include <algorithm>

#include "Elysium/Math.h"

namespace Elysium
{
    PhysicsSystem::PhysicsSystem(float acceleration, OrthographicCamera& camera) : 
        m_GravitationalAccel(acceleration),
        m_Camera(&camera),
        m_LogFile("Physics-Log.txt")
    {
    }

    PhysicsSystem::~PhysicsSystem()
    {
        m_LogFile.close();
    }

    PhysicsBody PhysicsSystem::createPhysicalBody(BodyType type, const char* name, float mass, const Vector2& initialPosition, const Vector2& size, 
        Collision_Callback callback)
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
        Vector2 min1 = body1.Position - body1.BroadSize / 2;
        Vector2 max1 = body1.Position + body1.BroadSize / 2;
        Vector2 min2 = body2.Position - body2.BroadSize / 2;
        Vector2 max2 = body2.Position + body2.BroadSize / 2;

        bool CollisionX = max1.x >= min2.x && min1.x <= max2.x;
        bool CollisionY = max1.y >= min2.y && min1.y <= max2.y;
        return CollisionX && CollisionY;
    }

    void PhysicsSystem::checkNarrowPhase(const PhysicalBody& body1, const PhysicalBody& body2, CollisionInfo& info)
    {
        std::vector<Vector2> normals;
        std::vector<Vector2>& vertices1 = body1.getVertices();
        std::vector<Vector2>& vertices2 = body2.getVertices();
        for (Vector2& normal : body1.getNormals())
            normals.push_back(normalize(normal));
        for (Vector2& normal : body2.getNormals())
            normals.push_back(normalize(normal));

        float overlap = 0.0f;
        Vector2 collisionNormal = { 0.0f, 0.0f };
        bool firstIsMax = false;
        for (Vector2& normal : normals)
        {
            float min1 =  std::numeric_limits<float>::max();
            float max1 = -std::numeric_limits<float>::max();
            for (Vector2& vertex : vertices1)
            {
                float projection = dot(vertex, normal);
                min1 = std::min(min1, projection);
                max1 = std::max(max1, projection);
            }

            float min2 =  std::numeric_limits<float>::max();
            float max2 = -std::numeric_limits<float>::max();
            for (Vector2& vertex : vertices2)
            {
                float projection = dot(vertex, normal);
                min2 = std::min(min2, projection);
                max2 = std::max(max2, projection);
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
        m_Bodies[body].Active = 0;
        m_InactiveBodies.push_back(body);
        std::sort(m_InactiveBodies.begin(), m_InactiveBodies.end(), std::greater());
    }

    void PhysicsSystem::onUpdate(Timestep ts)
    {
        m_Time += ts;

        for (uint32_t i = 0; i < m_Bodies.size(); i++)
        {
            if (m_Bodies[i].Type == BodyType::DYNAMIC && m_Bodies[i].Active > 0)
            {
                m_Bodies[i].Acceleration = m_Bodies[i].Force / m_Bodies[i].Mass;
                m_Bodies[i].Velocity = m_Bodies[i].Velocity + (m_Bodies[i].Impulse / m_Bodies[i].Mass) + (m_Bodies[i].Acceleration * (float)ts);
                m_Bodies[i].Position = m_Bodies[i].Position + (m_Bodies[i].Velocity * (float)ts);

                m_Bodies[i].Impulse = { 0.0f, 0.0f };

                if (Gravity && m_Bodies[i].Active != 2)
                {
                    m_Bodies[i].Force.y = -m_GravitationalAccel * m_Bodies[i].Mass;
                    m_Bodies[i].Active = 2;
                }
            }
        }

        for (uint32_t i = 0; i < m_Bodies.size(); i++)
        {
            if (m_Bodies[i].Active > 0)
            {
                for (uint32_t j = i + 1; j < m_Bodies.size(); j++)
                {
                    if (m_Bodies[j].Active > 0)
                    {
                        if (m_Bodies[i].Type == BodyType::STATIC && m_Bodies[j].Type == BodyType::STATIC)
                            continue;

                        if (checkBroadPhase(m_Bodies[i], m_Bodies[j]))
                        {
                            CollisionInfo info;
                            checkNarrowPhase(m_Bodies[i], m_Bodies[j], info);

                            if (info.Collision)
                            {
                                //m_LogFile << m_Time << ": NarrowPhaseCollision: " << m_Bodies[i].Name << ", " << m_Bodies[j].Name << std::endl;
                                //m_LogFile << m_Time << ": CollisionInfo Overlap: " << info.minOverlap << std::endl;
                                //m_LogFile << m_Time << ": CollisionInfo Normal: " << m_Bodies[i].Name << ": " << info.CollisionInfoPair.first.Normal.x << ", " << info.CollisionInfoPair.first.Normal.y << std::endl;
                                //m_LogFile << m_Time << ": CollisionInfo Normal: " << m_Bodies[j].Name << ": " << info.CollisionInfoPair.second.Normal.x << ", " << info.CollisionInfoPair.second.Normal.y << std::endl;

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

                                if (m_Bodies[i].Force.y >= 0.0f)
                                    m_Bodies[i].Active = 1;

                                if (m_Bodies[j].Force.y >= 0.0f)
                                    m_Bodies[j].Active = 1;

                                /***** Conservation of Momentum *****/
                                m_Bodies[i].Impulse += abs(m_Bodies[j].Velocity) * ObjectINormal;
                                m_Bodies[j].Impulse += abs(m_Bodies[i].Velocity) * ObjectJNormal;

                                /***** Normal Collision Impulse Response *****/
                                m_Bodies[i].Impulse += abs(m_Bodies[i].Velocity) * ObjectINormal * (m_Bodies[j].ElasticityCoefficient * 10.0f);
                                m_Bodies[j].Impulse += abs(m_Bodies[j].Velocity) * ObjectJNormal * (m_Bodies[i].ElasticityCoefficient * 10.0f);

                                /***** Friction *****/
                                Vector2 frictionDirection = { 0.0f, 0.0f };
                                if (m_Bodies[i].Velocity.x * m_Bodies[i].Velocity.x + m_Bodies[i].Velocity.y * m_Bodies[i].Velocity.y > 0)
                                {
                                    frictionDirection = { fabs(ObjectINormal.y),  fabs(ObjectINormal.x) };
                                    frictionDirection = -(frictionDirection * normalize(m_Bodies[i].Velocity));
                                    m_Bodies[i].Impulse += abs(m_Bodies[i].Velocity) * frictionDirection * m_Bodies[i].Mass * m_Bodies[j].getFrictionCoefficient() * (float)ts;
                                }
                                if (m_Bodies[j].Velocity.x * m_Bodies[j].Velocity.x + m_Bodies[j].Velocity.y * m_Bodies[j].Velocity.y > 0)
                                {
                                    frictionDirection = { fabs(ObjectJNormal.y) , fabs(ObjectJNormal.x) };
                                    frictionDirection = -(frictionDirection * normalize(m_Bodies[j].Velocity));
                                    m_Bodies[j].Impulse += abs(m_Bodies[j].Velocity) * frictionDirection * m_Bodies[j].Mass * m_Bodies[i].getFrictionCoefficient() * (float)ts;
                                }

                                //m_LogFile << m_Time << ": CollisionInfo Impulse: " << m_Bodies[i].Name << ": " << m_Bodies[i].Velocity.x << ", " << m_Bodies[i].Velocity.y << std::endl;
                                //m_LogFile << m_Time << ": CollisionInfo Impulse: " << m_Bodies[j].Name << ": " << m_Bodies[j].Velocity.x << ", " << m_Bodies[j].Velocity.y << std::endl;
                                //m_LogFile << m_Time << ": CollisionInfo Impulse: " << m_Bodies[i].Name << ": " << m_Bodies[i].Impulse.x << ", " << m_Bodies[i].Impulse.y << std::endl;
                                //m_LogFile << m_Time << ": CollisionInfo Impulse: " << m_Bodies[j].Name << ": " << m_Bodies[j].Impulse.x << ", " << m_Bodies[j].Impulse.y << std::endl;

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