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
/*
#include "IndexBuffer.h"
#include "Renderer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Shader.h"
*/

static void screenSaverLogic(float translation, bool& signX, bool& signY, float& x, float& y)
{
    if (!(x > -3.0f && x < 3.0f) &&
        !(y > -2.0f && y < 2.0f)) {
        signX = !signX;
        signY = !signY;
    }
    else if (!(x > -3.0f && x < 3.0f) &&
        (y > -2.0f && y < 2.0f)) {
        signX = !signX;
    }
    else if ((x > -3.0f && x < 3.0f) &&
        !(y > -2.0f && y < 2.0f)) {
        signY = !signY;
    }
    if (signX && signY) {
        x += translation;
        y += translation;
    }
    else if (signX && !signY) {
        x += translation;
        y -= translation;
    }
    else if (!signX && signY) {
        x -= translation;
        y += translation;
    }
    else {
        x += -translation;
        y += -translation;
    }
}

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

    float x = 0.0f;
    float y = 0.0f;

    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 proj = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, -1.0f, 1.0f);

    std::cout << glGetString(GL_VERSION) << std::endl;
    {
        float positions[] = {
           -0.5f, -0.5f, 0.0f, 0.0f,    // 0
            0.5f, -0.5f, 1.0f, 0.0f,    // 1
            0.5f,  0.5f, 1.0f, 1.0f,    // 2

           -0.5f,  0.5f, 0.0f, 1.0f,    // 3
            0.5f,  1.0f, 1.0f, 1.0f,    // 4

            /***SECOND***/
           -1.0f, -1.0f, 0.0f, 0.0f,    // 5
            1.0f, -1.0f, 1.0f, 0.0f,    // 6
            1.0f,  1.0f, 1.0f, 1.0f,    // 7
           -1.0f,  1.0f, 0.0f, 1.0f     // 8
            /***SECOND***/
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0,
            2, 4, 3
        };

        GL_ASSERT(glEnable(GL_BLEND));
        GL_ASSERT(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        VertexArray va;
        VertexBuffer vb(positions, 9 * 4 * sizeof(float));

        VertexBufferLayout layout;
        layout.push<float>(2);
        layout.push<float>(2);
        va.addBuffer(vb, layout);

        IndexBuffer ib(indices, 9);

        Shader shader("res/shaders/basic.shader");
        shader.bind();

        shader.setUniform1i("u_UseTexture", 1);
        Texture texture("res/texture/meadow.png");
        texture.bind(/*0*/);

        /***SECOND***/
        unsigned int sIndices[] = {
            5, 6, 7,
            7, 8, 5
        };

        VertexArray sVa;
        //VertexBuffer sVb(positions, 9 * 4 * sizeof(float));

        VertexBufferLayout sLayout;
        sLayout.push<float>(2);
        sLayout.push<float>(2);
        sVa.addBuffer(vb, sLayout);

        IndexBuffer sIb(sIndices, 6);

        Shader sShader("res/shaders/basic.shader");
        sShader.bind();

        /***TEXTURE*/
        sShader.setUniform1i("u_UseTexture", 0);
        Texture sTexture("res/texture/meadow.png");
        sTexture.bind(/*0*/);
        sShader.setUniform1i("u_Texture", 0);
        /***TEXTURE*/
        /***SECOND***/

        va.unBind();
        shader.unBind();
        vb.unBind();
        ib.unBind();

        /***SECOND***/
        sVa.unBind();
        sShader.unBind();
        //sVb.unBind();
        sIb.unBind();
        /***SECOND***/

        Renderer renderer;

        const char* glsl_version = "#version 130";
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
        ImGui::StyleColorsDark();

        float r = 0.0f;
        bool incR = true;

        bool signX = false;
        bool signY = false;
        float translation = 0.01f;

        glm::vec3 translationA(-1.0f, 0.0f, 0);
        glm::vec3 translationB(1.0f, 0.0f, 0);

        float viewX = 0.0f;

        bool show_demo_window = true;
        bool show_another_window = false;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.clear();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            /***CAMERA***/
            /*
            viewX -= 0.01f;
            view = glm::translate(glm::mat4(1.0f), glm::vec3(viewX, 0.0f, 0.0f));
            */
            /***CAMERA***/

            shader.bind();

            {
                glm::mat4 modelT = glm::translate(glm::mat4(1.0f), translationA);
                glm::mat4 mvpT = proj * view * modelT;
                shader.setUniformMat4f("u_MVP", mvpT);
                renderer.draw(va, ib, shader);
            }

            {
                glm::mat4 modelT = glm::translate(glm::mat4(1.0f), translationB);
                glm::mat4 mvpT = proj * view * modelT;
                shader.setUniformMat4f("u_MVP", mvpT);
                renderer.draw(va, ib, shader);
            }

            /***SECOND***/
            sShader.bind();
            sShader.setUniform4f("u_Color", r, 0.0f, 1.0f, 1.0f);
            renderer.draw(sVa, sIb, sShader);

            screenSaverLogic(translation, signX, signY, x, y);
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0));
            glm::mat4 mvp = proj * view * model;
            sShader.setUniformMat4f("u_MVP", mvp);

            if (r < 1.0f && incR) {
                r += 0.01f;
            }
            else if (r > 0 && !incR) {
                r -= 0.01f;
            }
            else {
                incR = !incR;
            }
            /***SECOND***/

            {
                ImGui::SliderFloat3("Translation A", &translationA.x, -3.0f, 3.0f);
                ImGui::SliderFloat3("Translation B", &translationB.x, -3.0f, 3.0f);
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    glfwTerminate();
    return 0;
}