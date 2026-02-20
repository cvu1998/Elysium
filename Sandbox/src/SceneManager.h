#pragma once

#include "Elysium.h"

#include "Scene.h"

class SceneManager : public Elysium::Layer
{
private:
    std::unique_ptr<Scene> m_CurrentScene;
    bool m_VSync = false;

public:
    SceneManager() = default;
    ~SceneManager();

    template<std::derived_from<Scene> T, typename ... Args> 
    void loadScene(Args&& ... args)
    {
        m_CurrentScene = std::make_unique<T>(std::forward<Args>(args) ...);
    }

    void unloadScene();

    void onUpdate(Elysium::Timestep ts);
    void onEvent(Elysium::Event& event);
};