#include "PhysicsSystem.h"

#include <iostream>
#include <limits>

#include "Elysium/Math.h"

namespace Elysium
{
    PhysicsSystem::PhysicsSystem(float acceleration, OrthographicCamera& camera) : 
        m_GravitationalAccel(acceleration),
        m_Camera(&camera)
    {
    }

    PhysicsSystem::~PhysicsSystem()
    {
    }

    bool PhysicsSystem::checkBroadPhase(const PhysicalObject* object1, const PhysicalObject* object2)
    {
        const Vector2& min1 = object1->Position - object1->BroadSize / 2;
        const Vector2& min2 = object2->Position - object1->BroadSize / 2;
        const Vector2& max1 = object1->Position + object1->BroadSize / 2;
        const Vector2& max2 = object2->Position + object1->BroadSize / 2;

        bool CollisionX = max1.x >= min2.x && min1.x <= max2.x;
        bool CollisionY = max1.y >= min2.y && min1.y <= max2.y;
        return CollisionX && CollisionY;
    }

    void PhysicsSystem::checkNarrowPhase(const PhysicalObject* object1, const PhysicalObject* object2, CollisionInfo& info)
    {
        std::vector<Vector2> normals;
        for (auto normal : object1->getNormals())
            normals.push_back(normalize(normal));
        for (auto normal : object2->getNormals())
            normals.push_back(normalize(normal));


        float min1 = std::numeric_limits<float>::min();
        float min2 = std::numeric_limits<float>::min();
        float max1 = std::numeric_limits<float>::max();
        float max2 = std::numeric_limits<float>::max();

        float minOverlap = 0.0f;
        Vector2 MTV;
        bool firstOrSecond = false;
        for (auto& normal : normals)
        {
           
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

    bool PhysicsSystem::areColliding(PhysicalObject* object1, PhysicalObject* object2)
    {
        auto it = m_CollisionMap.find(std::make_pair(object1, object2));
        if (it != m_CollisionMap.end())
            return it->second;
        return false;
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
                        //std::cout << "Collision!\n";

                        CollisionInfo info;
                        checkNarrowPhase(m_Objects[i], m_Objects[j],info);

                        if (info.Collision)
                        {
                            m_CollisionMap[std::make_pair(m_Objects[i], m_Objects[j])] = true;
                            m_CollisionMap[std::make_pair(m_Objects[j], m_Objects[i])] = true;

                            m_Objects[i]->onCollision();
                            m_Objects[j]->onCollision();
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