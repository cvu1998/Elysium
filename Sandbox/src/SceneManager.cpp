#include "SceneManager.h"

#include "Scenes/Connect4Scene.h"
#include "Scenes/Editor.h"
#include "Scenes/PerformanceScene.h"
#include "Scenes/SandboxScene.h"
#include "Scenes/SoccerScene.h"
#include "Scenes/TicTacToeScene.h"

SceneManager::~SceneManager()
{
    unloadScene();
}

void SceneManager::onUpdate(Elysium::Timestep ts)
{
    unsigned int width = Elysium::Application::Get().getWindow().getWidth();
    unsigned int height = Elysium::Application::Get().getWindow().getHeight();

    ImGui::Begin("Main Application");
    ImGui::Checkbox("VSync", &m_VSync);
    if (ImGui::Button("Editor"))
    {
        unloadScene();
        loadScene<Editor>(width, height);
    }
    if (ImGui::Button("Sandbox"))
    {
        unloadScene();
        Elysium::PhysicsSystem2D::Init<Elysium::UpdateDevice::CPU>();
        loadScene<SandboxScene>(width, height);
    }
    if (ImGui::Button("Stress Test"))
    {
        unloadScene();
        Elysium::PhysicsSystem2D::Init<Elysium::UpdateDevice::CPU_THREAD>();
        loadScene<PerformanceScene>(width, height);
    }
    if (ImGui::Button("Connect 4"))
    {
        unloadScene();
        Elysium::PhysicsSystem2D::Init<Elysium::UpdateDevice::CPU>();
        loadScene<Connect4Scene>(width, height);
    }
    if (ImGui::Button("Tic-Tac-Toe"))
    {
        unloadScene();
        loadScene<TicTacToeScene>(width, height);
    }
    if (ImGui::Button("Soccer"))
    {
        unloadScene();
        Elysium::PhysicsSystem2D::Init<Elysium::UpdateDevice::CPU>();
        loadScene<SoccerScene>(width, height);
    }
    ImGui::End();

    Elysium::Application::Get().getWindow().setVSync(m_VSync);

    if (m_CurrentScene) m_CurrentScene->onUpdate(ts);
}

void SceneManager::unloadScene()
{
    if (m_CurrentScene) m_CurrentScene->Cleanup();
}

void SceneManager::onEvent(Elysium::Event& event)
{
    if (m_CurrentScene) m_CurrentScene->onEvent(event);
}