#include "PhysicsSystem.h"

namespace Elysium
{
    PhysicsSystem::PhysicsSystem(float acceleration, OrthographicCamera& camera) : 
        m_GravitationalAccel(acceleration),
        m_Camera(&camera)
    {
        #ifdef _DEBUG
        Logfile.open("PhysicsSystem.log");
        #endif
    }

    PhysicsSystem::~PhysicsSystem()
    {
        #ifdef _DEBUG
        Logfile.close();
        #endif
    }

    bool PhysicsSystem::areIntersecting(const PhysicalObject* object1, const PhysicalObject* object2)
    {
        const glm::vec2& min1 = object1->getMinVertex();
        const glm::vec2& min2 = object2->getMinVertex();
        const glm::vec2& max1 = object1->getMaxVertex();
        const glm::vec2& max2 = object2->getMaxVertex();

        bool CollisionX = max1.x >= min2.x && min1.x <= max2.x;
        bool CollisionY = max1.y >= min2.y && min1.y <= max2.y;
        return CollisionX && CollisionY;
    }

    bool PhysicsSystem::checkFutureBoxCollision(const PhysicalObject* object1, const PhysicalObject* object2)
    {
        const glm::vec2& min1 = object1->getFuturePosition(object1->getMinVertex(), m_CurrentTimestep);
        const glm::vec2& min2 = object2->getFuturePosition(object2->getMinVertex(), m_CurrentTimestep);
        const glm::vec2& max1 = object1->getFuturePosition(object1->getMaxVertex(), m_CurrentTimestep);
        const glm::vec2& max2 = object2->getFuturePosition(object2->getMaxVertex(), m_CurrentTimestep);

        bool CollisionX = max1.x >= min2.x && min1.x <= max2.x;
        bool CollisionY = max1.y >= min2.y && min1.y <= max2.y;
        return CollisionX && CollisionY;
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
                    if (!areIntersecting(m_Objects[i], m_Objects[j]))
                    {
                        if (checkFutureBoxCollision(m_Objects[i], m_Objects[j]))
                        {
                            CollisionOccurence type = m_Objects[j]->getCollisionOccurence(m_Objects[i]);
                            if (type == CollisionOccurence::NONE)
                                type = m_Objects[i]->getCollisionOccurence(m_Objects[j]);

                            #ifdef _DEBUG
                            Logfile << "---------------------" << '\n';
                            Logfile << "Indexes: " << i << ", " << j << '\n';
                            Logfile << "Type: " << (int)type << '\n';
                            Logfile << "m_Objects[i]: " << m_Objects[i]->getPosition().x << ", " << m_Objects[i]->getPosition().y << '\n';
                            Logfile << "m_Objects[j]: " << m_Objects[j]->getPosition().x << ", " << m_Objects[j]->getPosition().y << '\n';
                            #endif

                            m_CollisionMap[std::make_pair(m_Objects[i], m_Objects[j])] = true;
                            m_CollisionMap[std::make_pair(m_Objects[j], m_Objects[i])] = true;

                            glm::vec2 normalI = -m_Objects[i]->Force;
                            m_Objects[i]->Force += normalI;

                            glm::vec2 normalJ = -m_Objects[j]->Force;
                            m_Objects[j]->Force += normalJ;

                            if (type == CollisionOccurence::TOP ||
                                type == CollisionOccurence::BOTTOM)
                            {
                                #ifdef _DEBUG
                                Logfile << "---------------------" << '\n';
                                Logfile << "Indexes: " << i << ", " << j << '\n';
                                Logfile << "CollisionY\n";
                                Logfile << "Before Force [i]: " << m_Objects[i]->Force.x << ", " << m_Objects[i]->Force.y << '\n';
                                Logfile << "Before Impulse [i]: " << m_Objects[i]->Impulse.x << ", " << m_Objects[i]->Impulse.y << '\n';
                                Logfile << "Acceleration [i]: " << m_Objects[i]->getAcceleration().x << ", " << m_Objects[i]->getAcceleration().y << '\n';
                                Logfile << "Velocity [i]: " << m_Objects[i]->getVelocity().x << ", " << m_Objects[i]->getVelocity().y << '\n';
                                #endif

                                m_Objects[i]->Impulse.y = -m_Objects[i]->getVelocity().y * m_Objects[i]->Mass * m_Objects[j]->getElasticityCoefficient();
                                m_Objects[j]->Impulse.y = -m_Objects[j]->getVelocity().y * m_Objects[j]->Mass * m_Objects[i]->getElasticityCoefficient();

                                float oldImpulseI = m_Objects[i]->Impulse.y;
                                float oldImpulseJ = m_Objects[j]->Impulse.y;
                                if (abs(m_Objects[j]->getVelocity().y) > 0)
                                    m_Objects[i]->Impulse.y -= oldImpulseJ;
                                if (abs(m_Objects[i]->getVelocity().y) > 0)
                                    m_Objects[j]->Impulse.y -= oldImpulseI;

                                if (abs(m_Objects[i]->getVelocity().x) > abs(m_Objects[i]->getVelocity().x) * m_Objects[j]->getFrictionCoefficient() || abs(m_Objects[i]->Impulse.x) > 0.0f)
                                    m_Objects[i]->Impulse.x -= m_Objects[i]->getVelocity().x * m_Objects[i]->getMass() * m_Objects[j]->getFrictionCoefficient() * ts;
                                else
                                    m_Objects[i]->Impulse.x = -m_Objects[i]->getVelocity().x * m_Objects[i]->getMass();

                                if (abs(m_Objects[j]->getVelocity().x) > abs(m_Objects[j]->getVelocity().x) * m_Objects[i]->getFrictionCoefficient() || abs(m_Objects[j]->Impulse.x) > 0.0f)
                                    m_Objects[j]->Impulse.x -= m_Objects[j]->getVelocity().x * m_Objects[j]->getMass() * m_Objects[i]->getFrictionCoefficient() * ts;
                                else
                                    m_Objects[j]->Impulse.x = -m_Objects[j]->getVelocity().x * m_Objects[j]->getMass();

                                #ifdef _DEBUG
                                Logfile << "Force [i]: " << m_Objects[i]->Force.x << ", " << m_Objects[i]->Force.y << '\n';
                                Logfile << "Impulse [i]: " << m_Objects[i]->Impulse.x << ", " << m_Objects[i]->Impulse.y << '\n';
                                Logfile << "Force [j]: " << m_Objects[j]->Force.x << ", " << m_Objects[i]->Force.y << '\n';
                                Logfile << "Impulse [j]: " << m_Objects[j]->Impulse.x << ", " << m_Objects[i]->Impulse.y << '\n';
                                Logfile << "---------------------" << '\n';
                                #endif
                            }
                            else if (type == CollisionOccurence::RIGHT ||
                                type == CollisionOccurence::LEFT)
                            {
                                #ifdef _DEBUG
                                Logfile << "---------------------" << '\n';
                                Logfile << "Indexes: " << i << ", " << j << '\n';
                                Logfile << "CollisionX\n";
                                Logfile << "Velocity [i]: " << m_Objects[i]->getVelocity().x << ", " << m_Objects[i]->getVelocity().y << '\n';
                                Logfile << "Velocity [j]: " << m_Objects[j]->getVelocity().x << ", " << m_Objects[j]->getVelocity().y << '\n';
                                #endif

                                m_Objects[i]->Impulse.x = -m_Objects[i]->getVelocity().x * m_Objects[i]->Mass * m_Objects[j]->getElasticityCoefficient();
                                m_Objects[j]->Impulse.x = -m_Objects[j]->getVelocity().x * m_Objects[j]->Mass * m_Objects[i]->getElasticityCoefficient();

                                float oldImpulseI = m_Objects[i]->Impulse.x;
                                float oldImpulseJ = m_Objects[j]->Impulse.x;
                                if (abs(m_Objects[j]->getVelocity().x) > 0)
                                    m_Objects[i]->Impulse.x -= oldImpulseJ;
                                if (abs(m_Objects[i]->getVelocity().x) > 0)
                                    m_Objects[j]->Impulse.x -= oldImpulseI;

                                #ifdef _DEBUG
                                Logfile << "Force [i]: " << m_Objects[i]->Force.x << ", " << m_Objects[i]->Force.y << '\n';
                                Logfile << "Impulse [i]: " << m_Objects[i]->Impulse.x << ", " << m_Objects[i]->Impulse.y << '\n';
                                Logfile << "Force [j]: " << m_Objects[j]->Force.x << ", " << m_Objects[i]->Force.y << '\n';
                                Logfile << "Impulse [j]: " << m_Objects[j]->Impulse.x << ", " << m_Objects[i]->Impulse.y << '\n';
                                Logfile << "---------------------" << '\n';
                                #endif
                            }
                            m_Objects[i]->onCollision();
                            m_Objects[j]->onCollision();
                        }
                        else
                        {
                            m_CollisionMap[std::make_pair(m_Objects[i], m_Objects[j])] = false;
                            m_CollisionMap[std::make_pair(m_Objects[j], m_Objects[i])] = false;
                        }
                    }
                    else
                    {
                        #ifdef _DEBUG
                        Logfile << "---------------------" << '\n';
                        Logfile << "Indexes: " << i << ", " << j << '\n';
                        Logfile << "Intersecting\n";
                        Logfile << "Before Force [i]: " << m_Objects[i]->Force.x << ", " << m_Objects[i]->Force.y << '\n';
                        Logfile << "Before Impulse [i]: " << m_Objects[i]->Impulse.x << ", " << m_Objects[i]->Impulse.y << '\n';
                        Logfile << "Acceleration [i]: " << m_Objects[i]->getAcceleration().x << ", " << m_Objects[i]->getAcceleration().y << '\n';
                        Logfile << "Velocity [i]: " << m_Objects[i]->getVelocity().x << ", " << m_Objects[i]->getVelocity().y << '\n';
                        #endif

                        m_Objects[i]->Impulse = ((m_Objects[i]->getPosition() - m_Objects[j]->getPosition()) * 0.5f) * m_Objects[i]->Mass * m_Objects[j]->getElasticityCoefficient();
                        m_Objects[j]->Impulse = ((m_Objects[j]->getPosition() - m_Objects[i]->getPosition()) * 0.5f) * m_Objects[j]->Mass * m_Objects[i]->getElasticityCoefficient();

                        #ifdef _DEBUG
                        Logfile << "Force [i]: " << m_Objects[i]->Force.x << ", " << m_Objects[i]->Force.y << '\n';
                        Logfile << "Impulse [i]: " << m_Objects[i]->Impulse.x << ", " << m_Objects[i]->Impulse.y << '\n';
                        Logfile << "Force [j]: " << m_Objects[j]->Force.x << ", " << m_Objects[i]->Force.y << '\n';
                        Logfile << "Impulse [j]: " << m_Objects[j]->Impulse.x << ", " << m_Objects[i]->Impulse.y << '\n';
                        Logfile << "---------------------" << '\n';
                        #endif
                    }
                }
            }
            #ifdef _DEBUG
            if (m_Objects[i]->getType() == ObjectType::DYNAMIC)
            {
                Logfile << "---------------------" << '\n';
                Logfile << "Index: " << i << '\n';
                Logfile << "Force [i]: " << m_Objects[i]->Force.x << ", " << m_Objects[i]->Force.y << '\n';
                Logfile << "Impulse [i]: " << m_Objects[i]->Impulse.x << ", " << m_Objects[i]->Impulse.y << '\n';
            }
            #endif

            m_Objects[i]->onUpdate(ts);

            #ifdef _DEBUG
            if (m_Objects[i]->getType() == ObjectType::DYNAMIC)
            {
                Logfile << "Position [i]: " << m_Objects[i]->getPosition().x << ", " << m_Objects[i]->getPosition().y << '\n';
                Logfile << "Velocity [i]: " << m_Objects[i]->getVelocity().x << ", " << m_Objects[i]->getVelocity().y << '\n';
                Logfile << "Acceleration [i]: " << m_Objects[i]->getAcceleration().x << ", " << m_Objects[i]->getAcceleration().y << '\n';
                Logfile << "---------------------" << '\n';
            }
            #endif
        }
        Renderer2D::endScene();
    }
}