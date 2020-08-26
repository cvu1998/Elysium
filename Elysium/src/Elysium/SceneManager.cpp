#include "SceneManager.h"

namespace Elysium
{
    SceneManager::SceneManager()
    {
    }

    SceneManager::~SceneManager()
    {
        if (m_CurrentScene)
            delete m_CurrentScene;
    }

    void SceneManager::onUpdate(Timestep ts)
    {
        if (m_CurrentScene)
            m_CurrentScene->onUpdate(ts);
    }

    void SceneManager::onEvent(Event& event)
    {
        if (m_CurrentScene)
            m_CurrentScene->onEvent(event);
    }

    void SceneManager::loadScene(Scene* scene)
    {
        if (m_CurrentScene)
        {
            delete m_CurrentScene;
        }
        m_CurrentScene = scene;
    }
}