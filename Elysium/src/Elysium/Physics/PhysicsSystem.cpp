#include "PhysicsSystem.h"

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

    bool PhysicsSystem::checkBroadPhase(const PhysicalObject* object1, const PhysicalObject* object2)
    {
        const Vector2& min1 = object1->Position - object1->BroadSize / 2;
        const Vector2& min2 = object2->Position - object2->BroadSize / 2;
        const Vector2& max1 = object1->Position + object1->BroadSize / 2;
        const Vector2& max2 = object2->Position + object2->BroadSize / 2;

        bool CollisionX = max1.x >= min2.x && min1.x <= max2.x;
        bool CollisionY = max1.y >= min2.y && min1.y <= max2.y;
        return CollisionX && CollisionY;
    }

    void PhysicsSystem::checkNarrowPhase(const PhysicalObject* object1, const PhysicalObject* object2, CollisionInfo& info)
    {
        std::vector<Vector2> normals;
        std::vector<Vector2>& vertices1 = object1->getVertices();
        std::vector<Vector2>& vertices2 = object2->getVertices();
        for (auto normal : object1->getNormals())
            normals.push_back(normalize(normal));
        for (auto normal : object2->getNormals())
            normals.push_back(normalize(normal));

        float overlap = 0.0f;
        Vector2 collisionNormal = { 0.0f, 0.0f };
        bool firstIsMax = false;
        for (auto& normal : normals)
        {
            float min1 =  std::numeric_limits<float>::max();
            float max1 = -std::numeric_limits<float>::max();
            for (auto& vertex : vertices1)
            {
                float projection = dot(vertex, normal);
                min1 = std::min(min1, projection);
                max1 = std::max(max1, projection);
            }

            float min2 =  std::numeric_limits<float>::max();
            float max2 = -std::numeric_limits<float>::max();
            for (auto& vertex : vertices2)
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

    void PhysicsSystem::addPhysicalObject(PhysicalObject* object)
    {
        if (object->getType() == ObjectType::DYNAMIC)
        {
            m_Objects.emplace(m_Objects.begin() + m_ObjectInsertIndex, object);
            m_ObjectInsertIndex++;
            return;
        }
        m_Objects.push_back(object);
        
    }

    void PhysicsSystem::removePhysicalObject(PhysicalObject* object)
    {
        auto it = std::find(m_Objects.begin(), m_Objects.end(), object);
        if (it != m_Objects.end())
        {
            if ((*it)->getType() == ObjectType::DYNAMIC)
                m_ObjectInsertIndex--;

            m_Objects.erase(it);
        }
    }

    const CollisionInfo& PhysicsSystem::areColliding(PhysicalObject* object1, PhysicalObject* object2)
    {
        auto it = m_CollisionMap.find(std::make_pair(object1, object2));
        if (it != m_CollisionMap.end())
            return it->second;
        return m_NoCollision;
    }

    void PhysicsSystem::onUpdate(Timestep ts)
    {
        m_Time += ts;
        m_CurrentTimestep = ts;

        for (unsigned int i = 0; i < m_ObjectInsertIndex; i++)
        {
            if (m_Objects[i]->getType() == ObjectType::DYNAMIC)
            {
                if (Gravity)
                    m_Objects[i]->Force.y = -m_GravitationalAccel * m_Objects[i]->Mass;
            }
        }

        m_CollisionMap.clear();
        for (unsigned int i = 0; i < m_Objects.size(); i++)
        {
            if (m_Objects[i]->getType() == ObjectType::DYNAMIC)
            {
                for (unsigned int j = i + 1; j < m_Objects.size(); j++)
                {
                    if (checkBroadPhase(m_Objects[i], m_Objects[j]))
                    {
                        CollisionInfo info;
                        checkNarrowPhase(m_Objects[i], m_Objects[j], info);

                        if (info.Collision)
                        {
                            m_LogFile << m_Time << ": NarrowPhaseCollision: " << m_Objects[i]->Name << ", " << m_Objects[j]->Name << std::endl;
                            m_LogFile << m_Time << ": CollisionInfo Overlap: " << info.minOverlap << std::endl;
                            m_LogFile << m_Time << ": CollisionInfo Normal: " << m_Objects[i]->Name << ": " << info.CollisionInfoPair.first.Normal.x << ", " << info.CollisionInfoPair.first.Normal.y << std::endl;
                            m_LogFile << m_Time << ": CollisionInfo Normal: " << m_Objects[j]->Name << ": " << info.CollisionInfoPair.second.Normal.x << ", " << info.CollisionInfoPair.second.Normal.y << std::endl;

                            Vector2 ObjectINormal = info.CollisionInfoPair.first.Normal;
                            Vector2 ObjectJNormal = info.CollisionInfoPair.second.Normal;

                            float magnitude1 = fabs(dot(m_Objects[i]->Velocity, ObjectINormal));
                            float magnitude2 = fabs(dot(m_Objects[j]->Velocity, ObjectJNormal));
                            if (magnitude1 > magnitude2)
                            {
                                if (m_Objects[i]->getType() == ObjectType::DYNAMIC)
                                    m_Objects[i]->Position = m_Objects[i]->Position + (info.minOverlap * ObjectINormal);
                                else
                                    m_Objects[j]->Position = m_Objects[j]->Position + (info.minOverlap * ObjectJNormal);
                            }
                            else
                            {
                                if (m_Objects[i]->getType() == ObjectType::DYNAMIC)
                                    m_Objects[j]->Position = m_Objects[j]->Position + (info.minOverlap * ObjectJNormal);
                                else
                                    m_Objects[i]->Position = m_Objects[i]->Position + (info.minOverlap * ObjectINormal);
                            }
                            /***** Normal Force *****/
                            m_Objects[i]->Force += abs(m_Objects[i]->Force) * ObjectINormal;
                            m_Objects[j]->Force += abs(m_Objects[j]->Force) * ObjectJNormal;

                            /***** Conservation of Momentum *****/
                            m_Objects[i]->Impulse += abs(m_Objects[j]->Velocity) * ObjectINormal;
                            m_Objects[j]->Impulse += abs(m_Objects[i]->Velocity) * ObjectJNormal;

                            /***** Normal Collision Impulse Response *****/
                            m_Objects[i]->Impulse += abs(m_Objects[i]->Velocity) * ObjectINormal * (m_Objects[j]->ElasticityCoefficient * 10.0f);
                            m_Objects[j]->Impulse += abs(m_Objects[j]->Velocity) * ObjectJNormal * (m_Objects[i]->ElasticityCoefficient * 10.0f);

                            /***** Friction *****/
                            Vector2 frictionDirection = { 0.0f, 0.0f };
                            if (m_Objects[i]->Velocity.x * m_Objects[i]->Velocity.x + m_Objects[i]->Velocity.y * m_Objects[i]->Velocity.y > 0)
                            {
                                frictionDirection = { fabs(ObjectINormal.y),  fabs(ObjectINormal.x) };
                                frictionDirection = -(frictionDirection * normalize(m_Objects[i]->Velocity));
                                m_Objects[i]->Impulse += abs(m_Objects[i]->Velocity) * frictionDirection * m_Objects[i]->Mass * m_Objects[j]->getFrictionCoefficient() * (float)ts;
                            }
                            if (m_Objects[j]->Velocity.x * m_Objects[j]->Velocity.x + m_Objects[j]->Velocity.y * m_Objects[j]->Velocity.y > 0)
                            {
                                frictionDirection = { fabs(ObjectJNormal.y) , fabs(ObjectJNormal.x) };
                                frictionDirection = -(frictionDirection * normalize(m_Objects[j]->Velocity));
                                m_Objects[j]->Impulse += abs(m_Objects[j]->Velocity) * frictionDirection * m_Objects[j]->Mass * m_Objects[i]->getFrictionCoefficient() * (float)ts;
                            }

                            m_LogFile << m_Time << ": CollisionInfo Impulse: " << m_Objects[i]->Name << ": " << m_Objects[i]->Velocity.x << ", " << m_Objects[i]->Velocity.y << std::endl;
                            m_LogFile << m_Time << ": CollisionInfo Impulse: " << m_Objects[j]->Name << ": " << m_Objects[j]->Velocity.x << ", " << m_Objects[j]->Velocity.y << std::endl;
                            m_LogFile << m_Time << ": CollisionInfo Impulse: " << m_Objects[i]->Name << ": " << m_Objects[i]->Impulse.x << ", " << m_Objects[i]->Impulse.y << std::endl;
                            m_LogFile << m_Time << ": CollisionInfo Impulse: " << m_Objects[j]->Name << ": " << m_Objects[j]->Impulse.x << ", " << m_Objects[j]->Impulse.y << std::endl;

                            m_CollisionMap[std::make_pair(m_Objects[i], m_Objects[j])] = info;
                            m_CollisionMap[std::make_pair(m_Objects[j], m_Objects[i])] = info;

                            m_Objects[i]->onCollision(m_Objects[j], info.CollisionInfoPair.first, info.CollisionInfoPair.second, ts);
                            m_Objects[j]->onCollision(m_Objects[i], info.CollisionInfoPair.second, info.CollisionInfoPair.first, ts);
                        }
                    }
                }
            }
        }

        Renderer2D::beginScene(*m_Camera);
        for (unsigned int i = 0; i < m_Objects.size(); i++)
        {
            m_Objects[i]->Draw();
            m_Objects[i]->onUpdate(ts);
        }
        Renderer2D::endScene();
    }
}