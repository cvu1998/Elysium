#pragma once

#include <Elysium.h>

class SandboxLayer : public Elysium::Layer
{
private:
    Elysium::OrthographicCamera m_Camera;
    std::vector<Texture> m_Textures;

    glm::vec3 m_CameraPosition;
    float m_CameraSpeed = 0.1f;
    float m_CameraRotation;
    float m_CameraRotationSpeed = 1.0f;

    float m_QuadPosition[2] = { -1.5f, -1.5f };
    float m_RotationSpeed = 0.0f;

public:
    SandboxLayer(unsigned int height, unsigned int width);
    ~SandboxLayer();

    void onUpdate() override;
    void onEvent(Elysium::Event& e) override;

    bool onKeyPressedEvent(Elysium::KeyPressedEvent& e);
};