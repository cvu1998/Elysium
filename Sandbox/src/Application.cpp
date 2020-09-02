#include "Scenes/Connect4Scene.h"
#include "Scenes/RLConnect4Scene.h"
#include "Scenes/PerformanceScene.h"
#include "Scenes/SandboxScene.h"
#include "Scenes/SoccerScene.h"
#include "Scenes/RLTTTScene.h"

class Application : public Elysium::Application
{
private:
    Elysium::Editor* m_Editor = nullptr;

    Connect4Scene* m_Connect4 = nullptr;
    RLConnect4Scene* m_RLConnect4 = nullptr;
    PerformanceScene* m_Test = nullptr;
    SandboxScene* m_Sandbox = nullptr;
    SoccerScene* m_Soccer = nullptr;
    RLTTTScene* m_TTT = nullptr;

    bool m_VSync = true;

public:
    Application(bool imgui=false) : Elysium::Application(imgui)
    {
        m_Window->setVSync(m_VSync);
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
            m_Editor = new Elysium::Editor(m_Window->getWidth(), m_Window->getHeight());
            m_SceneManager.loadScene(m_Editor);
        }
        if (ImGui::Button("Sandbox"))
        {
            m_Sandbox = new SandboxScene(m_Window->getWidth(), m_Window->getHeight());
            m_SceneManager.loadScene(m_Sandbox);
        }
        if (ImGui::Button("Stress Test"))
        {
            m_Test = new PerformanceScene(m_Window->getWidth(), m_Window->getHeight());
            m_SceneManager.loadScene(m_Test);;
        }
        if (ImGui::Button("Connect 4"))
        {
            m_Connect4 = new Connect4Scene(m_Window->getWidth(), m_Window->getHeight());
            m_SceneManager.loadScene(m_Connect4);
        }
        if (ImGui::Button("RL Tic-Tac-Toe"))
        {
            m_TTT = new RLTTTScene(m_Window->getWidth(), m_Window->getHeight());
            m_SceneManager.loadScene(m_TTT);
        }
        if (ImGui::Button("Soccer"))
        {
            m_Soccer = new SoccerScene(m_Window->getWidth(), m_Window->getHeight());
            m_SceneManager.loadScene(m_Soccer);
        }
        ImGui::End();

        m_Window->setVSync(m_VSync);
    }
};

int main(void)
{
    Application* application = new Application(true);
    application->Run();
    delete application;
    return 0;
}
