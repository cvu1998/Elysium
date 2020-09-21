#include "Editor.h"

#include <string>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Elysium/Input.h"
#include "Elysium/MouseCodes.h"

namespace Elysium
{
    Editor::Editor(unsigned int width, unsigned int height) :
        Scene("Editor"),
        m_WindowWidth(width),
        m_WindowHeight(height),
        m_CameraController((float)m_WindowWidth / (float)m_WindowHeight, 5.0f)
    {
        Renderer2D::setLineWidth(2.0f);
    }

    bool Editor::isWithinBounds(Vector2 position, Vector2 bottom, Vector2 top)
    {
        bool CollisionX = position.x >= bottom.x && position.x <= top.x;
        bool CollisionY = position.y >= bottom.y && position.y <= top.y;
        return CollisionX && CollisionY;
    }

    Vector2 Editor::getCursorPosition()
    {
        auto position = Elysium::Input::getMousePosition();

        return  m_CameraController.getCamera().getScreenToWorldPosition(m_WindowWidth, m_WindowHeight, position);
    }

    void Editor::saveSceneToFile()
    {
        if (!m_Data.Quads.empty())
        {
            std::string filename(m_Filename);
            filename += ".ely";
            std::ofstream sceneFile(filename);

            for (Quad& q : m_Data.Quads)
            {
                q.saveQuad(sceneFile);
            }
            sceneFile.close();
        }
    }

    void Editor::onUpdate(Timestep ts)
    {
        m_CameraController.onUpdate(ts);

        Vector2 cursorPosition = getCursorPosition();

        if (m_Current)
        {
            m_SameCurrent = true;
            Vector2 position = { m_Current->Position[0], m_Current->Position[1] };
            Vector2 size = { m_Current->Size[0] * 0.5f, m_Current->Size[1] * 0.5f };
            if (!isWithinBounds(cursorPosition, position - size, position + size))
            {
                m_Repeat = 0;
                m_CurrentIsMoving = false;
            }
        }

        if (Elysium::Input::isMouseButtonPressed(ELY_MOUSE_BUTTON_1))
        {
            if (m_Current)
            {
                Vector2 position = { m_Current->Position[0], m_Current->Position[1] };
                Vector2 size = { m_Current->Size[0] * 0.5f, m_Current->Size[1] * 0.5f };
                if (isWithinBounds(cursorPosition, position - size, position + size))
                {
                    if (m_CurrentIsMoving)
                    {
                        m_Current->Position[0] = cursorPosition.x;
                        m_Current->Position[1] = cursorPosition.y;
                    }
                }
            }
            for (Quad& q : m_Data.Quads)
            {
                Vector2 position = { q.Position[0], q.Position[1] };
                Vector2 size = { q.Size[0] * 0.5f, q.Size[1] * 0.5f };
                if (isWithinBounds(cursorPosition, position - size, position + size))
                {
                    if (!m_CurrentIsMoving)
                    {
                        m_Current = &q;
                        m_SameCurrent = false;
                    }
                }
            }
        }

        ImGui::Begin("Editor Layer");
        ImGui::InputText("Filename", m_Filename, 256);
        if (ImGui::Button("Save Scene to File"))
        {
            saveSceneToFile();
        }
        if (ImGui::Button("Open Scene from File"))
        {
            m_Data.Quads.clear();
            m_Current = nullptr;

            std::string filename = m_Filename;
            filename += ".ely";
            getSceneFromFile(filename.c_str(), m_Data.Quads);
        }
        if (ImGui::Button("Add Quad"))
        {
            m_Data.Quads.emplace_back();
            m_Current = &m_Data.Quads[m_Data.Quads.size() - 1];
            m_Current->Position[0] = m_CameraController.getCamera().getPosition().x;
            m_Current->Position[1] = m_CameraController.getCamera().getPosition().y;
        }
        if (ImGui::Button("Remove Current Quad"))
        {
            if (m_Current)
            {
                auto it = std::find(m_Data.Quads.begin(), m_Data.Quads.end(), *m_Current);
                if (it != m_Data.Quads.end())
                    m_Data.Quads.erase(it);
            }
            m_Current = nullptr;
        }
        if (ImGui::Button("Clear Scene"))
        {
            m_Data.Quads.clear();
            m_Current = nullptr;
        }
        ImGui::Text("Camera Bounds Height: %f", m_CameraController.getBoundsHeight());
        ImGui::Text("Camera Position: %f, %f", m_CameraController.getCamera().getPosition().x, m_CameraController.getCamera().getPosition().y);
        ImGui::Text("Cursor Position: %f, %f", cursorPosition.x, cursorPosition.y);
        ImGui::InputFloat("Set Camera Translation Speed", &m_TranslationSpeed);
        if (m_Current && m_SameCurrent)
        {
            char quadTag[64];
            strcpy_s(quadTag, 64, m_Current->Tag.data());
            ImGui::InputText("Object Tag", quadTag, 64);
            ImGui::InputFloat2("Position of quad", m_Current->Position);
            ImGui::SliderFloat("Rotation of quad", &(m_Current->Rotation), 0.0f, 360.0f);
            ImGui::InputFloat2("Size of quad", m_Current->Size);
            ImGui::ColorEdit4("Color of quad", m_Current->Color);
            m_Current->Tag = quadTag;
        }
        ImGui::End();

        #ifdef _DEBUG
        ImGui::Begin("Statistics");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Number of Draw Calls: %d", Renderer2D::getStats().DrawCount);
        ImGui::Text("Number of Quads: %d", Renderer2D::getStats().QuadCount);
        ImGui::Text("Number of Lines: %d", Renderer2D::getStats().LineCount);
        ImGui::End();
        #endif

        Renderer2D::resetStats();

        Renderer2D::beginScene(m_CameraController.getCamera());
        if (m_CameraController.getBoundsWidth() * m_CameraController.getBoundsHeight() <= 5000)
        {
            float leftBound = floor(m_CameraController.getCamera().getPosition().x - (m_CameraController.getBoundsWidth() * 0.5f));
            float rightBound = ceil(m_CameraController.getCamera().getPosition().x + (m_CameraController.getBoundsWidth() * 0.5f));
            for (float bottom = leftBound; bottom < rightBound; bottom++)
            {
                for (float left = leftBound; left < rightBound; left++)
                {
                    Renderer2D::drawLine({ left, bottom }, { left + 1.0f, bottom });
                    Renderer2D::drawLine({ left, bottom }, { left, bottom + 1.0f });
                }
            }
        }
        for (Quad& q : m_Data.Quads)
        {
            Renderer2D::drawQuadWithRotation({ q.Position[0], q.Position[1] }, { q.Size[0], q.Size[1] }, radians(q.Rotation),
                { q.Color[0], q.Color[1], q.Color[2], q.Color[3] });
        }
        Renderer2D::endScene();
    }

    void Editor::onEvent(Elysium::Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FUNCTION(Editor::onMousePressedEvent));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNCTION(Editor::onWindowResizeEvent));

        m_CameraController.onEvent(event);
    }

    bool Editor::onMousePressedEvent(Elysium::MouseButtonPressedEvent& event)
    {
        Vector2 cursorPosition = getCursorPosition();

        if (m_Current)
        {
            Vector2 position = { m_Current->Position[0], m_Current->Position[1] };
            Vector2 size = { m_Current->Size[0] * 0.5f, m_Current->Size[1] * 0.5f };
            if (isWithinBounds(cursorPosition, position - size, position + size))
            {
                if (++m_Repeat > 1)
                    m_CurrentIsMoving = true;
            }
        }

        return false;
    }

    bool Editor::onWindowResizeEvent(Elysium::WindowResizeEvent& event)
    {
        m_WindowWidth = event.getWidth();
        m_WindowHeight = event.getHeight();
        return false;
    }

    void Editor::getSceneFromFile(const char* filename, std::vector<Quad>& quads)
    {
        std::ifstream sceneFile(filename);
        if (sceneFile.is_open())
        {
            std::string line;
            QuadData dataType = QuadData::UNKNOWN;

            bool Complete = false;

            Quad quad;
            while (getline(sceneFile, line))
            {
                if (line.find("Tag") != std::string::npos)
                    dataType = QuadData::TAG;
                else if (line.find("Position") != std::string::npos)
                    dataType = QuadData::POSITION;
                else if (line.find("Size") != std::string::npos)
                    dataType = QuadData::SIZE;
                else if (line.find("Rotation") != std::string::npos)
                    dataType = QuadData::ROTATION;
                else if (line.find("Color") != std::string::npos)
                    dataType = QuadData::COLOR;

                size_t pos = (dataType == QuadData::POSITION || 
                    dataType == QuadData::SIZE || 
                    dataType == QuadData::COLOR) ? line.find(",") : 0;

                switch (dataType)
                {
                case QuadData::TAG:
                    quad.Tag = line.substr(4, std::string::npos);
                    break;
                case QuadData::POSITION:
                    quad.Position[0] = std::stof(line.substr(9, pos));
                    quad.Position[1] = std::stof(line.substr(pos + 2, std::string::npos));
                    break;
                case QuadData::SIZE:
                    quad.Size[0] = std::stof(line.substr(5, pos));
                    quad.Size[1] = std::stof(line.substr(pos + 2, std::string::npos));
                    break;
                case QuadData::ROTATION:
                    quad.Rotation = std::stof(line.substr(9, std::string::npos));
                    break;
                case QuadData::COLOR:
                    quad.Color[0] = std::stof(line.substr(6, pos));
                    for (size_t i = 1; i < 4; i++)
                    {
                        line = line.substr(pos + 2, std::string::npos);
                        pos = line.find(",");
                        quad.Color[i] = std::stof(line.substr(0, pos));
                    }
                    Complete = true;
                    break;
                }
                dataType = QuadData::UNKNOWN;
                
                if (Complete)
                {
                    Complete = false;
                    quads.push_back(quad);
                }
            }
            sceneFile.close();
        }
    }
}