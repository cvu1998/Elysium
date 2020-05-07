#include "PhysicsSystem.h"

namespace Elysium
{
    PhysicsSystem::PhysicsSystem(float acceleration, OrthographicCamera& camera) : 
        m_GravitationnalAccel(acceleration),
        m_Camera(&camera)
    {
        m_Logfile.open("PhysicsSystem_Logfile.txt");
    }

    PhysicsSystem::~PhysicsSystem()
    {
        m_Logfile.close();
    }

    void PhysicsSystem::addPhysicalObject(PhysicalObject* object)
    {
        if (object->getType() == ObjectType::DYNAMIC)
        {
            m_Objects.emplace(m_Objects.begin() + m_ObjectInsertIndex, object);
            m_ObjectInsertIndex++;
        }
        else
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

    bool PhysicsSystem::checkFutureBoxCollision(const PhysicalObject* object1, const PhysicalObject* object2)
    {
        float XObject1 = object1->getFuturePosition(m_CurrentTimestep).x;
        float XObject1HalfSize = object1->Size.x / 2;

        float XObject2 = object2->getFuturePosition(m_CurrentTimestep).x;
        float XObject2HalfSize = object2->Size.x / 2;

        bool CollisionX = 
            (XObject1 + XObject1HalfSize >=
                XObject2 - XObject2HalfSize &&
                XObject1 - XObject1HalfSize <=
                XObject2 + XObject2HalfSize);

        float YObject1 = object1->getFuturePosition(m_CurrentTimestep).y;
        float YObject1HalfSize = object1->Size.y / 2;

        float YObject2 = object2->getFuturePosition(m_CurrentTimestep).y;
        float YObject2HalfSize = object2->Size.y / 2;

        bool CollisionY = 
           (YObject1 + YObject1HalfSize >=
                YObject2 - YObject2HalfSize &&
                YObject1 - YObject1HalfSize <=
                YObject2 + YObject2HalfSize);

        return CollisionX && CollisionY;
    }

    void PhysicsSystem::onUpdate(Timestep ts)
    {
        m_Time += ts;
        m_CurrentTimestep = ts;

        Renderer2D::beginScene(*m_Camera);
        for (unsigned int i = 0; i < m_Objects.size(); i++)
        {
            if (m_Objects[i]->getType() == ObjectType::DYNAMIC)
            {
                m_Logfile << "--------------------" << '\n';
                m_Logfile << "Index: " << i << '\n';
                m_Logfile << "Gravity: " << m_Objects[i]->Acceleration.y << '\n';
                m_Objects[i]->Acceleration.y -= m_GravitationnalAccel;
                for (unsigned int j = i + 1; j < m_Objects.size(); j++)
                {
                    if (checkFutureBoxCollision(m_Objects[i], m_Objects[j]))
                    {
                        m_Logfile << "--------------------" << '\n';
                        m_Logfile << "Gravity: " << m_Objects[i]->Acceleration.y << '\n';

                        if (m_Objects[i]->getPosition().y > m_Objects[j]->getPosition().y + m_Objects[j]->Size.y / 2 ||
                            m_Objects[i]->getPosition().y < m_Objects[j]->getPosition().y - m_Objects[j]->Size.y / 2)
                            m_Objects[i]->Acceleration.y -= (m_Objects[i]->LastAcceleration.y + m_Objects[i]->Velocity.y / float(ts)) * 1.0f;

                        else if (m_Objects[i]->getPosition().x > m_Objects[j]->getPosition().x + m_Objects[j]->Size.x / 2 ||
                            m_Objects[i]->getPosition().x < m_Objects[j]->getPosition().x - m_Objects[j]->Size.x / 2)
                            m_Objects[i]->Acceleration.x -= (m_Objects[i]->LastAcceleration.x + m_Objects[i]->Velocity.x / float(ts)) * 1.0f;

                        m_Logfile << "LastAcceleration: " << m_Objects[i]->LastAcceleration.y << '\n';
                        m_Logfile << "Velocity: " << m_Objects[i]->Velocity.y << '\n';
                        m_Logfile << "Timestep: " << ts << '\n';
                        m_Logfile << "Velocity Compensator: " << m_Objects[i]->Velocity.y / float(ts) << '\n';
                        m_Logfile << "Acceleration: " << m_Objects[i]->Acceleration.y << '\n';
                        m_Logfile << "--------------------" << '\n';

                        m_Objects[i]->onCollision();
                    }
                    else
                    {
                        m_Logfile << "In Air, No Collision!" <<  '\n';
                        m_Logfile << "Position: { " << m_Objects[i]->getPosition().x << ", " <<  m_Objects[i]->getPosition().y << " }" << '\n';
                        m_Logfile << "Velocity: " << m_Objects[i]->Velocity.y << '\n';
                    }
                }
            }
            m_Objects[i]->onUpdate(ts);
        }
        Renderer2D::endScene();
    }
}