#include "SceneManager.h"

int main(void)
{
    Elysium::Application::Init();
    Elysium::Application::Get().getLayerStack().pushLayer<SceneManager>();
    Elysium::Application::Get().Run();
    Elysium::Application::Shutdown();
    return 0;
}