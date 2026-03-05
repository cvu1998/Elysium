#include "Application.h"

#include <algorithm>

#include "Elysium/Log.h"
#include "Elysium/Timestep.h"

#include "Elysium/Renderer/Renderer.h"

#include <commdlg.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace Elysium
{
    inline static Application* s_Instance = nullptr;

    Application::Application( 
        const std::string& title,
        unsigned int width,
        unsigned int height
    )
    {
        Log::Init();

        m_Window = std::unique_ptr<Window>(Window::Create( { title, width, height } ));
        m_Window->setEventCallback(BIND_EVENT_FUNCTION(Application::onEvent));

        if (m_Window->getStatus() == 0) ELY_CORE_ERROR("Glad Init Error!");
        
#if defined(_DEBUG)
        ELY_CORE_INFO("OpenGL version: {0}", glGetString(GL_VERSION));
#endif

        const char* glsl_version = "#version 130";
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(m_Window->getGLFWWindow(), true);
        ImGui_ImplOpenGL3_Init(glsl_version);
        ImGui::StyleColorsDark();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        Renderer::Init();

#if defined(_WIN64) || defined(_WIN32)
        WSADATA wsaData;
        int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (res != NO_ERROR) ELY_CORE_ERROR("WSAStartup failed with error!");
#endif
    }

    Application::~Application()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        Renderer::Shutdown();

        WSACleanup();
    }

    void Application::onEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNCTION(Application::onWindowCloseEvent));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNCTION(Application::onWindowResizeEvent));

        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
        {
            if (*--it)
            {
                (*it)->onEvent(event);
                if (event.Handled) break;
            }
        }
    }

    bool Application::onWindowCloseEvent(WindowCloseEvent& event)
    {
        m_Running.store(false, std::memory_order_release);
        return true;
    }

    bool Application::onWindowResizeEvent(WindowResizeEvent& event)
    {
        if (event.getWidth() == 0 || event.getHeight() == 0)
        {
            m_Minimized = true;
            return false;
        }
        m_Minimized = false;

        Render::setViewport(0, 0, event.getWidth(), event.getHeight());

        return false;
    }

    void Application::Init(
        const std::string& title, 
        unsigned int width,
        unsigned int height
    )
    {
        if (!s_Instance) s_Instance = new Application(title, width, height);
    }

    void Application::Shutdown()
    {
        delete s_Instance;
        s_Instance = nullptr;
    }

    Application& Application::Get()
    {
        return *s_Instance;
    }

    std::string Application::openFile(const char* filter)
    {
#if defined(_WIN64) || defined(_WIN32)
        OPENFILENAMEA ofn;
        CHAR szFile[260] = { 0 };
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().getWindow().getGLFWWindow());
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
        if (GetOpenFileNameA(&ofn) == TRUE)
        {
            return ofn.lpstrFile;
        }
#endif
        return std::string();
    }

    std::string Application::saveFile(const char* filter)
    {
#if defined(_WIN64) || defined(_WIN32)
        OPENFILENAMEA ofn;
        CHAR szFile[260] = { 0 };
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().getWindow().getGLFWWindow());
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
        if (GetSaveFileNameA(&ofn) == TRUE)
        {
            return ofn.lpstrFile;
        }
#endif
        return std::string();
    }

    void Application::Run()
    {
        while (m_Running.load(std::memory_order_acquire))
        {
            Render::Clear();

            float time = static_cast<float>(glfwGetTime());
            m_Timestep.store(time - m_LastFrameTime, std::memory_order_relaxed);
            m_LastFrameTime = time;
            m_FrameID.fetch_add(1, std::memory_order_relaxed);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            if (!m_Minimized)
            {
                for (auto& layer : m_LayerStack)
                {
                    if (layer) layer->onUpdate(m_Timestep);
                }
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            m_Window->onUpdate();
        }
    }
}