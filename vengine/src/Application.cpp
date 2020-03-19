#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <iostream>
#include <random>

#include "Renderer.h"
#include "Texture.h"
#include "VertexBufferLayout.h"

#include "test/ClearColor_Test.h"
#include "test/Texture2D_Test.h"
#include "test/ScreenSaver_Test.h"
#include "test/BatchRendering_Test.h"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    /* Create a windowed mode window and its OpenGL context */  
    window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
        std::cout << "Glew Init Error!" << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;

    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 proj = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, -1.0f, 1.0f);
    {
        Renderer& renderer = Renderer::getInstance();

        const char* glsl_version = "#version 130";
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
        ImGui::StyleColorsDark();

        bool show_demo_window = true;
        bool show_another_window = false;

        /***CAMERA***/
        /*
        float viewX = 0.0f;
        */
        /***CAMERA***/

        test::Test* currentTest = nullptr;
        test::TestMenu* testMenu = new test::TestMenu(currentTest);
        currentTest = testMenu;

        testMenu->registerTest<test::ClearColor_Test>("Clear Color");
        testMenu->registerTest<test::Texture2D_Test>("2D Texture");
        testMenu->registerTest<test::ScreenSaver_Test>("Screen Saver");
        testMenu->registerTest<test::BatchRendering_Test>("Batch Rendering");
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            renderer.clear();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            if (currentTest)
            {
                currentTest->onUpdate(0.0f);
                currentTest->onRender(proj, view);
                ImGui::Begin("Test");
                if (currentTest != testMenu && ImGui::Button("<--"))
                {
                    delete currentTest;
                    currentTest = testMenu;
                }
                currentTest->onImGuiRender();
                ImGui::End();
            }

            /***CAMERA***/
            /*
            viewX -= 0.01f;
            view = glm::translate(glm::mat4(1.0f), glm::vec3(viewX, 0.0f, 0.0f));
            */
            /***CAMERA***/

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);
            /* Poll for and process events */
            glfwPollEvents();
        }
        delete currentTest;
        if (currentTest != testMenu) 
            delete testMenu;
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
