#pragma once

#include <memory>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Elysium/LayerStack.h"
#include "Elysium/Window.h"

namespace Elysium 
{
    class Application final
    {
    private:
        std::unique_ptr<Window> m_Window;
        std::atomic<bool> m_Running = true;
        bool m_Minimized = false;
        float m_LastFrameTime = 0.0f;
        std::atomic<unsigned long long> m_FrameID = 0;
        std::atomic<Timestep> m_Timestep;

        LayerStack m_LayerStack;

    private:
        Application(
            const std::string& title, 
            unsigned int width,
            unsigned int height
        );
        ~Application();

        Application(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(const Application&) = delete;
        Application& operator=(Application&&) = delete;

        void onEvent(Event& event);

        bool onWindowCloseEvent(WindowCloseEvent& event);
        bool onWindowResizeEvent(WindowResizeEvent& event);

    public:
        static void Init(
            const std::string& title = "Elysium Engine",
            unsigned int width = 1280,
            unsigned int height = 720
        );
        static void Shutdown();

        static Application& Get();
        unsigned long long getFrameID() { return m_FrameID.load(); }
        Timestep getTimestep() { return m_Timestep.load(); }
        bool isRunning() { return m_Running;  }

        static std::string openFile(const char* filter);
        static std::string saveFile(const char* filter);

        Window& getWindow() { return *m_Window; }
        const Window& getWindow() const { return *m_Window; }

        inline LayerStack& getLayerStack() { return m_LayerStack;  }

        void Run();
    };
}

