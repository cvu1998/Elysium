#include "PhysicsSystem.h"

namespace Elysium
{
    PhysicsSystem::PhysicsSystem(float acceleration, OrthographicCamera& camera) : 
        m_GravitationalAccel(acceleration),
        m_Camera(&camera)
    {
        Logfile.open("PhysicsSystem.log");
    }

    PhysicsSystem::~PhysicsSystem()
    {
        Logfile.close();
    }

    bool PhysicsSystem::checkFutureBoxCollision(const PhysicalObject* object1, const PhysicalObject* object2)
    {
        const glm::vec2& min1 = object1->getFuturePosition(object1->VerticesPosition[0], m_CurrentTimestep);
        const glm::vec2& min2 = object2->getFuturePosition(object2->VerticesPosition[0], m_CurrentTimestep);
        const glm::vec2& max1 = object1->getFuturePosition(object1->VerticesPosition[2], m_CurrentTimestep);
        const glm::vec2& max2 = object2->getFuturePosition(object2->VerticesPosition[2], m_CurrentTimestep);

        bool CollisionX = max1.x >= min2.x && min1.x <= max2.x;
        bool CollisionY = max1.y >= min2.y && min1.y <= max2.y;
        return CollisionX && CollisionY;
    }

    void PhysicsSystem::addPhysicalObject(PhysicalObject* object)
    {
        object->m_ID = m_Objects.size() - 1;
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
        auto it = m_CollisionMap.find(std::make_pair(object1->getSystemID(), object2->getSystemID()));
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
                m_Objects[i]->Force.y = -m_GravitationalAccel * m_Objects[i]->Mass;
            }
        }

        Renderer2D::beginScene(*m_Camera);
        for (unsigned int i = 0; i < m_Objects.size(); i++)
        {
            m_Objects[i]->Draw();
            if (m_Objects[i]->getType() == ObjectType::DYNAMIC)
            {
                for (unsigned int j = i + 1; j < m_Objects.size(); j++)
                {
                    if (checkFutureBoxCollision(m_Objects[i], m_Objects[j]))
                    {
                        m_CollisionMap[std::make_pair(m_Objects[i]->getSystemID(), m_Objects[j]->getSystemID())] = true;
                        m_CollisionMap[std::make_pair(m_Objects[j]->getSystemID(), m_Objects[i]->getSystemID())] = true;

                        glm::vec2 normalI = -m_Objects[i]->Force;
                        m_Objects[i]->Force += normalI;

                        glm::vec2 normalJ = -m_Objects[j]->Force;
                        m_Objects[j]->Force += normalJ;

                        Logfile << "---------------------" << '\n';
                        Logfile << "Indexes: " << i << ", " << j << '\n';
                        Logfile << "m_Objects[i]: " << m_Objects[i]->VerticesPosition[0].y << ", " << m_Objects[i]->VerticesPosition[2].y << '\n';
                        Logfile << "m_Objects[j]: " << m_Objects[j]->VerticesPosition[0].y << ", " << m_Objects[j]->VerticesPosition[2].y << '\n';

                        if (m_Objects[j]->getCollisionOccurence(m_Objects[i]) == CollisionOccurence::TOP ||
                            m_Objects[j]->getCollisionOccurence(m_Objects[i]) == CollisionOccurence::BOTTOM)
                        {
                            Logfile << "---------------------" << '\n';
                            Logfile << "Indexes: " << i << ", " << j << '\n';
                            Logfile << "Before Force: " << m_Objects[i]->Force.x << ", " << m_Objects[i]->Force.y << '\n';
                            Logfile << "Before Impulse: " << m_Objects[i]->Impulse.x << ", " << m_Objects[i]->Impulse.y << '\n';
                            Logfile << "Acceleration: " << m_Objects[i]->getAcceleration().x << ", " << m_Objects[i]->getAcceleration().y << '\n';
                            Logfile << "Velocity: " << m_Objects[i]->getVelocity().x << ", " << m_Objects[i]->getVelocity().y << '\n';
                            Logfile << "CollisionY\n";

                            m_Objects[i]->Impulse.y = -m_Objects[i]->getVelocity().y * m_Objects[i]->Mass * m_Objects[j]->getElasticityCoefficient();
                            m_Objects[j]->Impulse.y = -m_Objects[j]->getVelocity().y * m_Objects[j]->Mass * m_Objects[i]->getElasticityCoefficient();

                            if (abs(m_Objects[i]->getVelocity().x) >= abs(m_Objects[i]->getVelocity().x) * m_Objects[j]->getFrictionCoefficient())
                                m_Objects[i]->Impulse.x -= m_Objects[i]->getVelocity().x * m_Objects[i]->getMass() * m_Objects[j]->getFrictionCoefficient();
                            else
                                m_Objects[i]->Impulse.x = -m_Objects[i]->getVelocity().x * m_Objects[i]->getMass();

                            if (abs(m_Objects[j]->getVelocity().x) >= abs(m_Objects[j]->getVelocity().x) * m_Objects[i]->getFrictionCoefficient())
                                m_Objects[j]->Impulse.x -= m_Objects[j]->getVelocity().x * m_Objects[j]->getMass() * m_Objects[i]->getFrictionCoefficient();
                            else
                                m_Objects[j]->Impulse.x = -m_Objects[j]->getVelocity().x * m_Objects[j]->getMass();

                            Logfile << "Force: " << m_Objects[i]->Force.x << ", " << m_Objects[i]->Force.y << '\n';
                            Logfile << "Impulse: " << m_Objects[i]->Impulse.x << ", " << m_Objects[i]->Impulse.y << '\n';
                            Logfile << "---------------------" << '\n';
                        }
                        else if (m_Objects[j]->getCollisionOccurence(m_Objects[i]) == CollisionOccurence::RIGHT ||
                            m_Objects[j]->getCollisionOccurence(m_Objects[i]) == CollisionOccurence::LEFT)
                        {
                            Logfile << "CollisionX\n";
                            Logfile << "Velocity: " << m_Objects[i]->getVelocity().x << ", " << m_Objects[i]->getVelocity().y << '\n';

                            m_Objects[i]->Impulse.x = -m_Objects[i]->getVelocity().x * m_Objects[i]->Mass * m_Objects[j]->getElasticityCoefficient();
                            m_Objects[j]->Impulse.x = -m_Objects[j]->getVelocity().x * m_Objects[j]->Mass * m_Objects[i]->getElasticityCoefficient();

                            Logfile << "Force: " << m_Objects[j]->Force.x << ", " << m_Objects[i]->Force.y << '\n';
                            Logfile << "Impulse: " << m_Objects[j]->Impulse.x << ", " << m_Objects[i]->Impulse.y << '\n';
                            Logfile << "---------------------" << '\n';
                        }
                        m_Objects[i]->onCollision();
                        m_Objects[j]->onCollision();
                    }
                    else
                    {
                        m_CollisionMap[std::make_pair(m_Objects[i]->getSystemID(), m_Objects[j]->getSystemID())] = false;
                        m_CollisionMap[std::make_pair(m_Objects[j]->getSystemID(), m_Objects[i]->getSystemID())] = false;
                    }
                }
            }
            if (m_Objects[i]->getType() == ObjectType::DYNAMIC)
            {
                Logfile << "---------------------" << '\n';
                Logfile << "Index: " << i << '\n';
                Logfile << "Force: " << m_Objects[i]->Force.x << ", " << m_Objects[i]->Force.y << '\n';
                Logfile << "Impulse: " << m_Objects[i]->Impulse.x << ", " << m_Objects[i]->Impulse.y << '\n';
            }

            m_Objects[i]->onUpdate(ts);

            if (m_Objects[i]->getType() == ObjectType::DYNAMIC)
            {
                Logfile << "Position: " << m_Objects[i]->getPosition().x << ", " << m_Objects[i]->getPosition().y << '\n';
                Logfile << "Velocity: " << m_Objects[i]->getVelocity().x << ", " << m_Objects[i]->getVelocity().y << '\n';
                Logfile << "Acceleration: " << m_Objects[i]->getAcceleration().x << ", " << m_Objects[i]->getAcceleration().y << '\n';
                Logfile << "---------------------" << '\n';
            }
        }
        Renderer2D::endScene();
    }
}