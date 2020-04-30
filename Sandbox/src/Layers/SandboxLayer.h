#pragma once

#include <Elysium.h>

class SandboxLayer : public Elysium::Layer
{
private:
    Elysium::OrthographicCamera m_Camera;
    std::vector<Texture> m_Textures;

    glm::vec3 m_CameraPosition;
    float m_CameraSpeed = 2.0f;
    float m_CameraRotation;
    float m_CameraRotationSpeed = 45.0f;

    float m_QuadPosition[2] = { -1.5f, -1.5f };
    float m_RotationSpeed = 0.0f;

public:
    SandboxLayer(unsigned int height, unsigned int width);
    ~SandboxLayer();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& e) override;

    bool onKeyPressedEvent(Elysium::KeyPressedEvent& event);
    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};