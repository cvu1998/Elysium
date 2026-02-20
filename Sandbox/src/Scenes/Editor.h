#pragma once

#include <fstream>
#include <vector>

#include "Elysium.h"

#include "Scene.h"

enum class QuadData
{
    UNKNOWN = -1,
    TAG = 0,
    POSITION = 1,
    SIZE = 2,
    ROTATION = 3,
    COLOR = 4
};

struct Quad
{
    std::string Tag = "Quad";
    Elysium::Vector2 Position = { 0.0f, 0.0f };
    Elysium::Vector2 Size = { 2.0f, 2.0f };
    float Rotation = 0.0f;
    Elysium::Vector4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };

    void saveQuad(std::ofstream& sceneFile)
    {
        sceneFile << "-Quad\n";
        sceneFile << "--Tag=" << Tag << '\n'
            << "--Position=" << Position.x << ", " << Position.y << '\n'
            << "--Size=" << Size.x << ", " << Size.y << '\n'
            << "--Rotation=" << Rotation << '\n'
            << "--Color=" << Color.r << ", " << Color.g << ", " << Color.b << ", " << Color.a << '\n';
    }

    bool operator==(const Quad& quad)
    {
        return (this->Tag == quad.Tag &&
            this->Position.x == quad.Position.x &&
            this->Position.y == quad.Position.y);
    }
};

struct SceneData
{
    std::vector<Quad> Quads;
};

class Editor : public Scene
{
private:
    unsigned int m_WindowWidth;
    unsigned int m_WindowHeight;

    Elysium::OrthographicCameraController m_CameraController;
    float m_TranslationSpeed = 5.0f;

    Quad* m_Current = nullptr;
    unsigned int m_Repeat = 0;
    bool m_CurrentIsMoving = false;
    bool m_SameCurrent = false;

    Elysium::Vector2 m_Marker = { 0.0f, 0.0f };

    SceneData m_Data;

    bool m_ViewportFocus = false;
    Elysium::Vector2 m_ViewportPosition = { 0.0f, 0.0f };
    Elysium::Vector2 m_ViewportSize = { 0.0f, 0.0f };
    std::unique_ptr<Elysium::Framebuffer> m_Framebuffer;

private:
    bool isWithinBounds(Elysium::Vector2 position, Elysium::Vector2 bottom, Elysium::Vector2 top);

    Elysium::Vector2 getCursorPosition();

    void Serialize(const char* filepath);

public:
    Editor(unsigned int width, unsigned int height);
    ~Editor();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onMousePressedEvent(Elysium::MouseButtonPressedEvent& event);
    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);

    static bool Deserialize(const char* filepath, std::vector<Quad>& quads);
};