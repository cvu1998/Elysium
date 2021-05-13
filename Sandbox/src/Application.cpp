#include "Scenes/Connect4Scene.h"
#include "Scenes/FlappyBirdScene.h"
#include "Scenes/PerformanceScene.h"
#include "Scenes/SandboxScene.h"
#include "Scenes/SoccerScene.h"
#include "Scenes/RLTTTScene.h"

int main(void)
{
    bool VSync = false;
    Elysium::Application* application = Elysium::Application::createApplication([&]()
        {
            unsigned int width = application->getWindow().getWidth();
            unsigned int height = application->getWindow().getHeight();

            ImGui::Begin("Main Application");
            ImGui::Checkbox("VSync", &VSync);
            ImGui::ColorEdit4("Clear Color", application->ClearColor);
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
                application->SceneManager.loadScene(new RLTTTScene(width, height));
            }
            if (ImGui::Button("Flappy Bird"))
            {
                application->SceneManager.unloadScene();
                application->SceneManager.loadScene(new FlappyBirdScene(width, height));
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