#include "Editor.h"

#include <string>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "Elysium/Application.h"
#include "Elysium/ImGuiUtility.h"
#include "Elysium/Input.h"
#include "Elysium/Log.h"
#include "Elysium/MouseCodes.h"

namespace Elysium
{
    Editor::Editor(unsigned int width, unsigned int height) :
        Scene("Editor"),
        m_WindowWidth(width),
        m_WindowHeight(height),
        m_CameraController((float)m_WindowWidth / (float)m_WindowHeight, 5.0f)
    {
        FramebufferSpecification specification;
        specification.Width = width;
        specification.Height = height;
        m_Framebuffer = std::make_unique<Framebuffer>(specification);

        Renderer2D::setLineWidth(2.0f);
    }

    Editor::~Editor()
    {
        Renderer::setViewport(0, 0, m_WindowWidth, m_WindowHeight);
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
        position.first -= m_ViewportPosition.x;
        position.second -= m_ViewportPosition.y;

        return m_CameraController.getCamera().getScreenToWorldPosition((unsigned int)m_ViewportSize.x, (unsigned int)m_ViewportSize.y, position);
    }

    void Editor::Serialize(const char* filepath)
    {
        if (!m_Data.Quads.empty())
        {
            std::ofstream sceneFile(filepath);

            sceneFile << "Scene" << '\n';
            for (Quad& q : m_Data.Quads)
            {
                q.saveQuad(sceneFile);
            }
            sceneFile.close();
        }
    }

    void Editor::onUpdate(Timestep ts)
    {
        if (m_ViewportFocus)
            m_CameraController.onUpdate(ts);

        Vector2 cursorPosition = getCursorPosition();

        if (Elysium::Input::isMouseButtonPressed(ELY_MOUSE_BUTTON_1) &&
            Elysium::Input::isKeyPressed(ELY_KEY_LEFT_SHIFT))
            m_Marker = cursorPosition;

        if (m_Current)
        {
            m_SameCurrent = true;
            Vector2 size = m_Current->Size * 0.5f;
            if (!isWithinBounds(cursorPosition, m_Current->Position - size, m_Current->Position + size))
            {
                m_Repeat = 0;
                m_CurrentIsMoving = false;
            }
        }

        if (Elysium::Input::isMouseButtonPressed(ELY_MOUSE_BUTTON_1))
        {
            if (m_Current)
            {
                Vector2 size = m_Current->Size * 0.5f;
                if (isWithinBounds(cursorPosition, m_Current->Position - size, m_Current->Position + size))
                {
                    if (m_CurrentIsMoving)
                    {
                        m_Current->Position.x = cursorPosition.x;
                        m_Current->Position.y = cursorPosition.y;
                    }
                }
            }
            for (Quad& q : m_Data.Quads)
            {
                Vector2 size = q.Size * 0.5f;
                if (isWithinBounds(cursorPosition, q.Position - size, q.Position + size))
                {
                    if (!m_CurrentIsMoving)
                    {
                        m_Current = &q;
                        m_SameCurrent = false;
                    }
                }
            }
        }

        m_Framebuffer->Bind();
        Renderer::Clear();
        Renderer2D::resetStats();
        Renderer2D::beginScene(m_CameraController.getCamera());
        if (m_CameraController.getBoundsWidth() * m_CameraController.getBoundsHeight() <= 5000.0f)
        {
            float leftBound = floor(m_CameraController.getCamera().getPosition().x - (m_CameraController.getBoundsWidth() * 0.5f));
            float rightBound = ceil(m_CameraController.getCamera().getPosition().x + (m_CameraController.getBoundsWidth() * 0.5f));
            float bottomBound = floor(m_CameraController.getCamera().getPosition().y - (m_CameraController.getBoundsHeight() * 0.5f));
            float topBound = ceil(m_CameraController.getCamera().getPosition().y + (m_CameraController.getBoundsHeight() * 0.5f));
            for (float bottom = bottomBound; bottom < topBound; bottom++)
            {
                for (float left = leftBound; left < rightBound; left++)
                {
                    Renderer2D::drawLine({ left, bottom, 0.0f }, { left + 1.0f, bottom, 0.0f }, { 0.5f, 0.5f, 0.5f, 1.0f });
                    Renderer2D::drawLine({ left, bottom, 0.0f }, { left, bottom + 1.0f, 0.0f }, { 0.5f, 0.5f, 0.5f, 1.0f });
                }
            }
        }
        for (Quad& q : m_Data.Quads)
        {
            Renderer2D::drawQuadWithRotation(q.Position, q.Size, glm::radians(q.Rotation), q.Color);
        }
        Renderer2D::endScene();
        m_Framebuffer->Unbind();

        ImGuiUtility::createImGuiDockspace([&]()
        {
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("New"))
                    {
                        m_Data.Quads.clear();
                        m_Current = nullptr;
                    }
                    if (ImGui::MenuItem("Open Scene"))
                    {
                        std::string filepath = Application::openFile("Elysium Scene (*.elysium)\0*.elysium\0");
                        if (!(filepath.empty()))
                        {
                            m_Data.Quads.clear();
                            m_Current = nullptr;

                            Deserialize(filepath.c_str(), m_Data.Quads);
                        }
                    }
                    if (ImGui::MenuItem("Save Scene"))
                    {
                        std::string filepath = Application::saveFile("Elysium Scene (*.elysium)\0*.elysium\0");
                        if (!(filepath.empty()))
                            Serialize(filepath.c_str());
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            ImGui::Begin("Editor");
            if (ImGui::Button("Add Quad"))
            {
                m_Data.Quads.emplace_back();
                m_Current = &m_Data.Quads[m_Data.Quads.size() - 1];
                m_Current->Position = m_Marker;
            }
            if (ImGui::Button("Remove Quad"))
            {
                if (m_Current)
                {
                    auto it = std::find(m_Data.Quads.begin(), m_Data.Quads.end(), *m_Current);
                    if (it != m_Data.Quads.end())
                        m_Data.Quads.erase(it);
                }
                m_Current = nullptr;
            }
            ImGui::Text("Camera Position: %f, %f", m_CameraController.getCamera().getPosition().x, m_CameraController.getCamera().getPosition().y);
            ImGui::Text("Cursor Position: %f, %f", cursorPosition.x, cursorPosition.y);
            ImGuiUtility::drawImGuiLabelWithColumn("Camera Translation Speed", [&]()
                {
                    ImGui::InputFloat("##", &m_TranslationSpeed);
                    m_CameraController.CameraTranslationSpeed = m_TranslationSpeed;
                }, 190.0f);

            if (m_Current && m_SameCurrent)
            {
                ImGui::Begin("Properties");
                char quadTag[64];
                strcpy_s(quadTag, 64, m_Current->Tag.c_str());
                ImGuiUtility::drawImGuiLabelWithColumn("Tag", [&]()
                    {
                        ImGui::InputText("##", quadTag, 64);
                        m_Current->Tag = quadTag;
                    });
                ImGuiUtility::drawImGuiLabelWithColumn("Position", [&]()
                    {
                        ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
                        ImGui::DragFloat("##X", &m_Current->Position.x, 0.1f, 0.0f, 0.0f, "%.2f");
                        ImGui::PopItemWidth();
                        ImGui::SameLine();
                        ImGui::DragFloat("##Y", &m_Current->Position.y, 0.1f, 0.0f, 0.0f, "%.2f");
                        ImGui::PopItemWidth();
                    });
                ImGuiUtility::drawImGuiLabelWithColumn("Rotation", [&]()
                    {
                        ImGui::SliderFloat("##", &(m_Current->Rotation), 0.0f, 360.0f);
                    });
                ImGuiUtility::drawImGuiLabelWithColumn("Size", [&]()
                    {
                        ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
                        float size = m_Current->Size.x;
                        ImGui::DragFloat("##X", &m_Current->Size.x, 0.1f, 0.0f, 0.0f, "%.2f");
                        if (!(m_Current->Size.x > 0.0f))
                            m_Current->Size.x = size;
                        ImGui::PopItemWidth();
                        ImGui::SameLine();
                        size = m_Current->Size.y;
                        ImGui::DragFloat("##Y", &m_Current->Size.y, 0.1f, 0.0f, 0.0f, "%.2f");
                        if (!(m_Current->Size.y > 0.0f))
                            m_Current->Size.y = size;
                        ImGui::PopItemWidth();
                    });
                ImGuiUtility::drawImGuiLabelWithColumn("Color", [&]()
                    {
                        ImGui::ColorEdit4("##", (float*)&m_Current->Color);
                    });
                ImGui::End();
            }
            ImGui::End();

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
            ImGui::Begin("Viewport");
            m_ViewportFocus = ImGui::IsWindowFocused();
            ImVec2 viewportPosition = ImGui::GetWindowPos();
            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
            m_ViewportPosition = { viewportPosition.x, viewportPosition.y };
            if (m_ViewportSize != *((Vector2*)&viewportPanelSize))
            {
                m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
                m_Framebuffer->Resize((unsigned int)m_ViewportSize.x, (unsigned int)m_ViewportSize.y);

                m_CameraController.resizeBounds(m_ViewportSize.x, m_ViewportSize.y);
            }
            unsigned int textureID = m_Framebuffer->getColorAttachmentID();
            ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
            ImGui::End();
            ImGui::PopStyleVar();
        });
    }

    void Editor::onEvent(Elysium::Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FUNCTION(Editor::onMousePressedEvent));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNCTION(Editor::onWindowResizeEvent));

        if (m_ViewportFocus)
        {
            m_CameraController.onEvent(event);
            m_CameraController.resizeBounds(m_ViewportSize.x, m_ViewportSize.y);
        }
    }

    bool Editor::onMousePressedEvent(Elysium::MouseButtonPressedEvent& event)
    {
        Vector2 cursorPosition = getCursorPosition();

        if (m_Current)
        {
            Vector2 size = m_Current->Size * 0.5f;
            if (isWithinBounds(cursorPosition, m_Current->Position - size, m_Current->Position + size))
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

    bool Editor::Deserialize(const char* filepath, std::vector<Quad>& quads)
    {
        std::ifstream sceneFile(filepath);
        if (sceneFile.is_open())
        {
            std::string line;
            getline(sceneFile, line);
            if (line != "Scene")
                return false;
            QuadData dataType = QuadData::UNKNOWN;

            bool Complete = false;

            Quad quad;
            while (getline(sceneFile, line))
            {
                if (line.find("Quad") != std::string::npos)
                    continue;
                else if (line.find("Tag") != std::string::npos)
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
                    quad.Tag = line.substr(6, std::string::npos);
                    break;
                case QuadData::POSITION:
                    quad.Position.x = std::stof(line.substr(11, pos));
                    quad.Position.y = std::stof(line.substr(pos + 2, std::string::npos));
                    break;
                case QuadData::SIZE:
                    quad.Size.x = std::stof(line.substr(7, pos));
                    quad.Size.y = std::stof(line.substr(pos + 2, std::string::npos));
                    break;
                case QuadData::ROTATION:
                    quad.Rotation = std::stof(line.substr(11, std::string::npos));
                    break;
                case QuadData::COLOR:
                    quad.Color.r = std::stof(line.substr(8, pos));

                    line = line.substr(pos + 2, std::string::npos);
                    pos = line.find(",");
                    quad.Color.g = std::stof(line.substr(0, pos));

                    line = line.substr(pos + 2, std::string::npos);
                    pos = line.find(",");
                    quad.Color.b = std::stof(line.substr(0, pos));

                    line = line.substr(pos + 2, std::string::npos);
                    pos = line.find(",");
                    quad.Color.a = std::stof(line.substr(0, pos));
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
            return true;
        }
        return false;
    }
}