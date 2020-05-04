#include "SandboxLayer.h"

SandboxLayer::SandboxLayer(bool* runSandbox, unsigned int width, unsigned int height) : Layer("Sandbox"),
    m_RunSandbox(runSandbox),
    m_CameraController((float)width / (float)height, 3.0f),
    m_ParticleSystem(100, m_CameraController.getCamera()),
    m_PhysicsSystem(0.1f, m_CameraController.getCamera())
{
    Renderer2D::Init();

    m_Textures.reserve(3);
    m_Textures.emplace_back("res/texture/meadow.png");
    m_Textures.emplace_back("res/texture/Vader.png");
    m_Textures.emplace_back("res/texture/alec.png");

    m_Particle.Position = { 0.0f, 0.0f };
    m_Particle.Velocity = { 0.0f, 0.0f };
    m_Particle.VelocityVariation = { 1.0f, 1.0f };
    m_Particle.RotationSpeed = 180.0f;

    m_Particle.ColorBegin = { 0.0f, 0.0f, 1.0f, 1.0f };
    m_Particle.ColorEnd = { 0.0f, 0.0f, 0.0f, 1.0f };
    //m_Particle.TextureID = m_Textures[2].getRendererID();

    m_Particle.SizeBegin = 0.5f, m_Particle.SizeEnd = 0.0f, m_Particle.SizeVariation = 0.3f;

    m_Particle.LifeTime = 3.0f;

    // ---------------------------------------------------------------------------------- //

    m_Particle2.Position = { 0.0f, 0.0f };
    m_Particle2.Velocity = { 0.0f, 0.0f };
    m_Particle2.VelocityVariation = { 1.0f, 1.0f };
    m_Particle2.RotationSpeed = 180.0f;

    m_Particle2.ColorBegin = { 1.0f, 0.0f, 1.0f, 1.0f };
    m_Particle2.ColorEnd = { 0.0f, 0.0f, 0.0f, 1.0f };
    //m_Particle.TextureID = m_Textures[2].getRendererID();

    m_Particle2.SizeBegin = 0.5f, m_Particle.SizeEnd = 0.0f, m_Particle.SizeVariation = 0.3f;

    m_Particle2.LifeTime = 3.0f;

    m_Object.InitialPosition = { 0.0f, 5.0f };
    m_Object.Size = { 1.0f, 2.0f };
    m_Object.InitialVelocity = { 0.0f, 0.0f };
    m_Object.Acceleration = { 0.0f, 0.0f };

    m_Object.Color = { 0.0f, 0.0f, 1.0f, 1.0f };
    //m_Object.TextureID = m_Textures[2].getRendererID();

    m_PhysicsSystem.addPhysicalObject(&m_Object);
}

SandboxLayer::~SandboxLayer()
{
    Renderer2D::Shutdown();
}

void SandboxLayer::onUpdate(Elysium::Timestep ts)
{
    if (*m_RunSandbox)
    {
        m_CameraController.onUpdate(ts);

        Renderer2D::beginScene(m_CameraController.getCamera());

        for (float x = -4.0f; x < 4.0f; x += 0.1f)
        {
            for (float y = -3.0f; y < 3.0f; y += 0.1f)
            {
                glm::vec4 gradient = { (x + 4.0f) / 8.0f, (y + 3.0f) / 12.0f, 1.0f, 1.0f };
                Renderer2D::drawQuadWithRotation({ x + 0.025f , y + 0.025f }, { 0.05f, 0.05f }, glm::radians(m_RotationSpeed), gradient);
            }
        }

        glm::vec4 black = { 0.0f, 0.0f, 0.0f, 1.0f };
        glm::vec4 white = { 1.0f, 1.0f, 1.0f, 1.0f };

        for (int x = -2; x < 2; x++)
        {
            for (int y = -2; y < 2; y++)
            {
                glm::vec4 color = (x + y) % 2 == 0 ? white : black;
                Renderer2D::drawQuad({ x + 0.5f , y + 0.5f }, { 1.0f, 1.0f }, color);
            }
        }
        glm::vec4 color = { 0.0f, 1.0f, 1.0f, 1.0f };
        Renderer2D::drawQuadWithRotation({ -1.5f, 1.5f }, { 1.0f, 1.0f }, glm::radians(m_RotationSpeed), color);
        Renderer2D::drawQuad({ 1.5f, 1.5f }, { 1.0f, 1.0f }, color);

        Renderer2D::drawQuadWithRotation({ 1.5f, -1.5f }, { 1.0f, 1.0f }, glm::radians(m_RotationSpeed), m_Textures[1].getRendererID());
        Renderer2D::drawQuad({ m_QuadPosition[0], m_QuadPosition[1] }, { 1.0f, 1.0f }, m_Textures[0].getRendererID(), 
            { m_QuadColor[0], m_QuadColor[1], m_QuadColor[2], m_QuadColor[3] });

        Renderer2D::drawQuadWithRotation({ 0.0f, 0.0f }, { 2.0f, 1.0f }, glm::radians(m_RotationSpeed), color);

        m_RotationSpeed += 120.0f * ts;
        Renderer2D::endScene();

        auto [x, y] = Elysium::Input::getMousePosition();
        auto width = Elysium::Application::Get().getWindow().getWidth();
        auto height = Elysium::Application::Get().getWindow().getHeight();

        auto pos = m_CameraController.getCamera().getPosition();
        x = (x / width) * m_CameraController.getBoundsWidth() - m_CameraController.getBoundsWidth() * 0.5f;
        y = m_CameraController.getBoundsHeight() * 0.5f - (y / height) * m_CameraController.getBoundsHeight();
        m_Particle.Position = { x + pos.x, y + pos.y };
        m_Particle2.Position = { x + pos.x, y + pos.y };

        for (int i = 0; i < 5; i++)
        {
            m_ParticleSystem.Emit(m_Particle);
            m_ParticleSystem.Emit(m_Particle2);
        }
        m_ParticleSystem.OnUpdate(ts);

        m_Object.Rotation = m_RotationSpeed / ts;
        m_PhysicsSystem.onUpdate(ts);

        ImGui::Begin("First Quad Controls");
        ImGui::DragFloat2("First Quad Position", m_QuadPosition, 0.1f);
        ImGui::ColorEdit4("Quad Color", m_QuadColor);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Number of Draw Calls: %d", Renderer2D::getStats().DrawCount);
        ImGui::Text("Number of Quads: %d", Renderer2D::getStats().QuadCount);
        ImGui::End();

        Renderer2D::resetStats();
    }
}

void SandboxLayer::onEvent(Elysium::Event& event)
{
    m_CameraController.onEvent(event);

    Elysium::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Elysium::KeyPressedEvent>(BIND_EVENT_FUNCTION(SandboxLayer::onKeyPressedEvent));
    dispatcher.Dispatch<Elysium::WindowResizeEvent>(BIND_EVENT_FUNCTION(SandboxLayer::onWindowResizeEvent));
}

bool SandboxLayer::onKeyPressedEvent(Elysium::KeyPressedEvent& event)
{
    return false;
}

bool SandboxLayer::onWindowResizeEvent(Elysium::WindowResizeEvent& event)
{
    return false;
}