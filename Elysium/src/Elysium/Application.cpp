#include "Application.h"

#include <algorithm>

#include "Elysium/Log.h"
#include "Elysium/Renderer/Renderer.h"
#include "Elysium/Timestep.h"

#include <commdlg.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace Elysium
{
    Application* Application::s_Instance = nullptr;

    Application::Application(const UpdateFunction& updateFunction, 
        const std::string& title, unsigned int width, unsigned int height, 
        bool imgui) : m_Function(updateFunction), m_ImGui(imgui)
    {
        Log::Init();

        m_Window = std::unique_ptr<Window>(Window::Create( { title, width, height } ));
        m_Window->setEventCallback(BIND_EVENT_FUNCTION(Application::onEvent));

        if (m_Window->getStatus() == 0)
            ELY_CORE_ERROR("Glad Init Error!");
        
        #ifdef _DEBUG
        ELY_CORE_INFO("OpenGL version: {0}", glGetString(GL_VERSION));
        #endif

        m_Run = std::bind(&Application::runWithoutImGui, this);
        if (m_ImGui)
        {
            const char* glsl_version = "#version 130";
            ImGui::CreateContext();
            ImGui_ImplGlfw_InitForOpenGL(m_Window->getGLFWWindow(), true);
            ImGui_ImplOpenGL3_Init(glsl_version);
            ImGui::StyleColorsDark();

            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

            m_Run = std::bind(&Application::runWithImGui, this);
        }
        Renderer::Init();

        WSADATA wsaData;
        int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (res != NO_ERROR) 
            ELY_CORE_ERROR("WSAStartup failed with error!");
    }

    Application::~Application()
    {
        SceneManager.unloadScene();
        if (m_ImGui)
        {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }
        Renderer::Shutdown();

        WSACleanup();
    }

    void Application::onEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNCTION(Application::onWindowCloseEvent));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNCTION(Application::onWindowResizeEvent));

        SceneManager.onEvent(event);

        for (auto it = LayerStack.end(); it != LayerStack.begin();)
        {
            if (*--it)
            {
                (*it)->onEvent(event);
                if (event.Handled)
                    break;
            }
        }
    }

    bool Application::onWindowCloseEvent(WindowCloseEvent& event)
    {
        m_Running = false;
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

    void Application::runWithImGui()
    {
        while (m_Running)
        {
            Render::Clear();

            float time = (float)glfwGetTime();
            Timestep timestep = time - m_LastFrameTime;
            m_LastFrameTime = time;

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            //if (!m_Minimized)
            {
                m_Function();

                SceneManager.onUpdate(timestep);

                for (Layer* layer : LayerStack)
                {
                    if (layer)
                        layer->onUpdate(timestep);
                }
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            m_Window->onUpdate();
        }
    }

    void Application::runWithoutImGui()
    {
        while (m_Running)
        {
            Render::Clear();

            float time = (float)glfwGetTime();
            Timestep timestep = time - m_LastFrameTime;
            m_LastFrameTime = time;

            if (!m_Minimized)
            {
                m_Function();

                SceneManager.onUpdate(timestep);

                for (Layer* layer : LayerStack)
                {
                    if (layer)
                        layer->onUpdate(timestep);
                }
            }

            m_Window->onUpdate();
        }
    }

    Application* Application::createApplication(const UpdateFunction& function, 
        const std::string& title, 
        unsigned int width, unsigned int height,
        bool imgui)
    {
        if (!s_Instance)
            s_Instance = new Application(function, title, width, height, imgui);
        return s_Instance;
    }

    std::string Application::openFile(const char* filter)
    {
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
        return std::string();
    }

    std::string Application::saveFile(const char* filter)
    {
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
        return std::string();
    }

    void Application::Run()
    {
        m_Run();
    }

    void Application::Shutdown()
    {
        delete s_Instance;
        s_Instance = nullptr;
    }
}