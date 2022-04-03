#include "Scenes/Connect4Scene.h"
#include "Scenes/GridWorldScene.h"
#include "Scenes/PerformanceScene.h"
#include "Scenes/SandboxScene.h"
#include "Scenes/SoccerScene.h"
#include "Scenes/TicTacToeScene.h"

int main(void)
{
    bool VSync = false;
    Elysium::Application* application = Elysium::Application::createApplication([&]()
        {
            unsigned int width = application->getWindow().getWidth();
            unsigned int height = application->getWindow().getHeight();

            ImGui::Begin("Main Application");
            ImGui::Checkbox("VSync", &VSync);
            if (ImGui::Button("Editor"))
            {
                application->SceneManager.unloadScene();
                application->SceneManager.loadScene(new Elysium::Editor(width, height));
            }
            if (ImGui::Button("Sandbox"))
            {
                application->SceneManager.unloadScene();
                application->SceneManager.loadScene(new SandboxScene(width, height));
            }
            if (ImGui::Button("Stress Test"))
            {
                application->SceneManager.unloadScene();
                application->SceneManager.loadScene(new PerformanceScene(width, height));
            }
            if (ImGui::Button("Connect 4"))
            {
                application->SceneManager.unloadScene();
                application->SceneManager.loadScene(new Connect4Scene(width, height));
            }
            if (ImGui::Button("Tic-Tac-Toe"))
            {
                application->SceneManager.unloadScene();
                application->SceneManager.loadScene(new TicTacToeScene(width, height));
            }
            if (ImGui::Button("Grid World"))
            {
                application->SceneManager.unloadScene();
                application->SceneManager.loadScene(new GridWorldScene(width, height));
            }
            if (ImGui::Button("Soccer"))
            {
                application->SceneManager.unloadScene();
                application->SceneManager.loadScene(new SoccerScene(width, height));
            }
            ImGui::End();

            application->getWindow().setVSync(VSync);
        });

    application->Run();
    application->Shutdown();
    return 0;
}