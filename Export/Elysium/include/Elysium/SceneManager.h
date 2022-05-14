#pragma once

#include "Scene.h"

namespace Elysium
{
    class SceneManager final
    {
    private:
        Scene* m_CurrentScene = nullptr;

    public:
        SceneManager() = default;

        void onUpdate(Timestep ts);
        void onEvent(Event& event);

        void unloadScene();
        void loadScene(Scene* scene);
        
        inline bool isRunningScene() { return m_CurrentScene != nullptr; }
    };
}