#include "SceneManager.h"

namespace Elysium
{
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

    void SceneManager::unloadScene()
    {
        if (m_CurrentScene)
        {
            delete m_CurrentScene;
            m_CurrentScene = nullptr;
        }
    }

    void SceneManager::loadScene(Scene* scene)
    {
        unloadScene();
        m_CurrentScene = scene;
    }
}