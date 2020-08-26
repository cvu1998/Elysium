#pragma once

#include "Scene.h"

namespace Elysium
{
    class SceneManager
    {
    private:
        Scene* m_CurrentScene = nullptr;

    public:
        SceneManager();
        ~SceneManager();

        void onUpdate(Timestep ts);
        void onEvent(Event& event);

        void loadScene(Scene* scene);
    };
}