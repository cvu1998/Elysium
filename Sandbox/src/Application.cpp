#include "Scenes/CellGrowthScene.h"
#include "Scenes/Connect4Scene.h"
#include "Scenes/FlappyBirdScene.h"
#include "Scenes/PerformanceScene.h"
#include "Scenes/SandboxScene.h"
#include "Scenes/SoccerScene.h"
#include "Scenes/RLTTTScene.h"

class Application : public Elysium::Application
{
private:
    bool m_VSync = true;

public:
    Application() : Elysium::Application()
    {
    }

    ~Application()
    {
    }

    void ApplicationLogic() override
    {
        ImGui::Begin("Main Application");
        ImGui::Checkbox("VSync", &m_VSync);
        ImGui::ColorEdit4("Clear Color", m_ClearColor);
        if (ImGui::Button("Editor"))
        {
            m_SceneManager.unloadScene();
            m_SceneManager.loadScene(new Elysium::Editor(m_Window->getWidth(), m_Window->getHeight()));
        }
        if (ImGui::Button("Sandbox"))
        {
            m_SceneManager.unloadScene();
            m_SceneManager.loadScene(new SandboxScene(m_Window->getWidth(), m_Window->getHeight()));
        }
        if (ImGui::Button("Stress Test"))
        {
            m_SceneManager.unloadScene();
            m_SceneManager.loadScene(new PerformanceScene(m_Window->getWidth(), m_Window->getHeight()));;
        }
        if (ImGui::Button("Connect 4"))
        {
            m_SceneManager.unloadScene();
            m_SceneManager.loadScene(new Connect4Scene(m_Window->getWidth(), m_Window->getHeight()));
        }
        if (ImGui::Button("Tic-Tac-Toe"))
        {
            m_SceneManager.unloadScene();
            m_SceneManager.loadScene(new RLTTTScene(m_Window->getWidth(), m_Window->getHeight()));
        }
        if (ImGui::Button("Flappy Bird"))
        {
            m_SceneManager.unloadScene();
            m_SceneManager.loadScene(new FlappyBirdScene(m_Window->getWidth(), m_Window->getHeight()));
        }
        if (ImGui::Button("Soccer"))
        {
            m_SceneManager.unloadScene();
            m_SceneManager.loadScene(new SoccerScene(m_Window->getWidth(), m_Window->getHeight()));
        }
        if (ImGui::Button("Cell Growth"))
        {
            m_SceneManager.unloadScene();
            m_SceneManager.loadScene(new CellGrowthScene(m_Window->getWidth(), m_Window->getHeight()));
        }
        ImGui::End();

        m_Window->setVSync(m_VSync);
    }
};

int main(void)
{
    Application* application = new Application();
    application->Run();
    delete application;
    return 0;
}