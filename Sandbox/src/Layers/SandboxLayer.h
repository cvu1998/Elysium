#pragma once

#include <Elysium.h>

class SandboxLayer : public Elysium::Layer
{
private:
    bool* m_RunSandbox;

    Elysium::OrthographicCameraController m_CameraController;
    std::vector<Texture> m_Textures;

    Elysium::ParticleProperties m_Particle;
    Elysium::ParticleProperties m_Particle2;
    Elysium::ParticleSystem m_ParticleSystem;

    float m_QuadColor[4] = { 0.75f, 0.0f, 0.75f, 1.0f };
    float m_QuadPosition[2] = { -1.5f, -1.5f };
    float m_RotationSpeed = 0.0f;

public:
    SandboxLayer(bool* runSandbox, unsigned int height, unsigned int width);
    ~SandboxLayer();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;

    bool onKeyPressedEvent(Elysium::KeyPressedEvent& event);
    bool onWindowResizeEvent(Elysium::WindowResizeEvent& event);
};