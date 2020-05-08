#include "PhysicsSystem.h"

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
            (XObject1 + XObject1HalfSize * cos(glm::radians(object1->Rotation)) >=
                XObject2 - XObject2HalfSize * cos(glm::radians(object2->Rotation)) &&
                XObject1 - XObject1HalfSize * cos(glm::radians(object1->Rotation)) <=
                XObject2 + XObject2HalfSize * cos(glm::radians(object2->Rotation)));

        float YObject1 = object1->getFuturePosition(m_CurrentTimestep).y;
        float YObject1HalfSize = object1->Size.y / 2;

        float YObject2 = object2->getFuturePosition(m_CurrentTimestep).y;
        float YObject2HalfSize = object2->Size.y / 2;

        bool CollisionY =
            (YObject1 + YObject1HalfSize * sin(glm::radians(object1->Rotation + 90.0f)) >=
                YObject2 - YObject2HalfSize * sin(glm::radians(object1->Rotation + 90.0f)) &&
                YObject1 - YObject1HalfSize * sin(glm::radians(object1->Rotation + 90.0f)) <=
                YObject2 + YObject2HalfSize * sin(glm::radians(object1->Rotation + 90.0f)));

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
                m_Objects[i]->Acceleration.y -= m_GravitationalAccel;
                m_Objects[i]->setGravitationalAccel(-m_GravitationalAccel);
                for (unsigned int j = i + 1; j < m_Objects.size(); j++)
                {
                    if (checkFutureBoxCollision(m_Objects[i], m_Objects[j]))
                    {
                        m_Objects[i]->Rotation = m_Objects[j]->Rotation;
                        if (m_Objects[j]->getCollisionOccurence(m_Objects[i]) == CollisionOccurence::TOP ||
                            m_Objects[j]->getCollisionOccurence(m_Objects[i]) == CollisionOccurence::BOTTOM)
                        {
                            m_Objects[i]->Acceleration.y -= (m_Objects[i]->getLastAcceleration().y + m_Objects[i]->Velocity.y / float(ts)) * m_Objects[j]->getElasticityCoefficient();
                            if (m_Objects[i]->Velocity.x != 0.0f)
                            {
                                if (m_Objects[i]->Velocity.x > 0.0f)
                                    m_Objects[i]->Acceleration.x -= abs(m_Objects[i]->getLastAcceleration().y) * m_Objects[j]->getFrictionCoefficient();
                                else
                                    m_Objects[i]->Acceleration.x += abs(m_Objects[i]->getLastAcceleration().y) * m_Objects[j]->getFrictionCoefficient();
                            }
                        }
                        else if (m_Objects[j]->getCollisionOccurence(m_Objects[i]) == CollisionOccurence::RIGHT ||
                            m_Objects[j]->getCollisionOccurence(m_Objects[i]) == CollisionOccurence::LEFT)
                        {
                            m_Objects[i]->Acceleration.x -= (m_Objects[i]->getLastAcceleration().x + m_Objects[i]->Velocity.x / float(ts)) *  m_Objects[j]->getElasticityCoefficient();
                        }
                        m_Objects[i]->onCollision();
                        m_Objects[j]->onCollision();
                    }
                }
            }
            m_Objects[i]->onUpdate(ts);
        }
        Renderer2D::endScene();
    }
}