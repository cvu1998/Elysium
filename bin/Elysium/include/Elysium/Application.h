#pragma once

#include <memory>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Elysium/LayerStack.h"
#include "Elysium/SceneManager.h"
#include "Elysium/Window.h"

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
        bool m_Minimized = false;
        float m_LastFrameTime = 0.0f;
        float m_ClearColor[4];
        LayerStack m_LayerStack;
        SceneManager m_SceneManager;

    protected:
        Application(const std::string& title="Elysium Engine", unsigned int width=1280, unsigned int height=720, 
            bool imgui=true);
        ~Application();

        virtual void onEvent(Event& event) final;

        bool onWindowCloseEvent(WindowCloseEvent& event);
        bool onWindowResizeEvent(WindowResizeEvent& event);

        virtual void ApplicationLogic() = 0;

    public:
        static Application& Get() { return *s_Instance; }
        static std::string openFile(const char* filter);
        static std::string saveFile(const char* filter);

        virtual Window& getWindow() final { return *m_Window; }
        virtual const Window& getWindow() const final { return *m_Window; }

        void setClearColor(float r, float g, float b, float a);

        virtual void Run() final;
    };
}

