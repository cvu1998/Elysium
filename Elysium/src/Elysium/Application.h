#pragma once

#include <memory>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "LayerStack.h"
#include "Window.h"

namespace Elysium 
{
    class Application
    {
    private:
        static Application* s_Instance;

    protected:
        std::unique_ptr<Window> m_Window;
        bool m_ImGui;
        bool m_Running = true;
        LayerStack m_LayerStack;

        Application(bool imgui=false);
        ~Application();

        virtual void onEvent(Event& event) final;

        virtual void pushLayer(Layer* layer) final;
        virtual void pushOverlay(Layer* overlay) final;

        bool onWindowCloseEvent(WindowCloseEvent& event);

        virtual void ApplicationLogic() = 0;

    public:
        static Application& Get() { return *s_Instance; }

        virtual Window& getWindow() final { return *m_Window; }

        virtual void Run() final;
        virtual void RunWithImGui() final;
    };
}

