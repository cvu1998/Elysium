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
    class Application final
    {
    private:
        static Application* s_Instance;

        std::unique_ptr<Window> m_Window;
        bool m_ImGui;
        bool m_Running = true;
        bool m_Minimized = false;
        float m_LastFrameTime = 0.0f;

        using UpdateFunction = std::function<void()>;

        UpdateFunction m_Run;
        UpdateFunction m_Function;

    public:
        float ClearColor[4];
        LayerStack LayerStack;
        SceneManager SceneManager;

    private:
        Application(const UpdateFunction& function,
            const std::string& title, 
            unsigned int width, unsigned int height,
            bool imgui);
        ~Application();

        Application(const Application&) = delete;
        Application(Application&&) = delete;

        Application& operator=(const Application&) = delete;
        Application& operator=(Application&&) = delete;

        void onEvent(Event& event);

        bool onWindowCloseEvent(WindowCloseEvent& event);
        bool onWindowResizeEvent(WindowResizeEvent& event);

        void runWithImGui();
        void runWithoutImGui();

    public:
        static Application* createApplication(const UpdateFunction& function,
            const std::string& title = "Elysium Engine",
            unsigned int width = 1280, unsigned int height = 720,
            bool imgui = true);

        static Application& Get() { return *s_Instance; }
        static std::string openFile(const char* filter);
        static std::string saveFile(const char* filter);

        Window& getWindow() { return *m_Window; }
        const Window& getWindow() const { return *m_Window; }

        void Run();
        void Shutdown();
    };
}

