#include "SandboxLayer.h"

SandboxLayer::SandboxLayer(bool* runSandbox, unsigned int width, unsigned int height) : Layer("Sandbox"),
    m_RunSandbox(runSandbox),
    m_CameraController((float)width / (float)height, 3.0f),
    m_ParticleSystem(100, m_CameraController.getCamera()),
    m_PhysicsSystem(5.0f, m_CameraController.getCamera()),
    m_Player({ 0.0f, 10.0f }, { 2.0f, 2.0f }, 1.0f),
    m_Dynamic({ 0.0f, 15.0f }, { 2.0f, 2.0f }, 1.25f),
    m_Ground({ 0.0f, 0.0f }, {1000.0f, 2.0f}),
    m_Box({ 5.0f, 2.0f }, { 2.0f, 2.0f }),
    m_Ceiling({ 0.0f, 20.0f }, {1000.0f, 2.0f})
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

    m_Particle.SizeBegin = 0.5f, m_Particle.SizeEnd = 0.0f, m_Particle.SizeVariation = 0.3f;

    m_Particle.LifeTime = 3.0f;

    // ---------------------------------------------------------------------------------- //

    m_Particle2.Position = { 0.0f, 0.0f };
    m_Particle2.Velocity = { 0.0f, 0.0f };
    m_Particle2.VelocityVariation = { 1.0f, 1.0f };
    m_Particle2.RotationSpeed = 180.0f;

    m_Particle2.ColorBegin = { 1.0f, 0.0f, 1.0f, 1.0f };
    m_Particle2.ColorEnd = { 0.0f, 0.0f, 0.0f, 1.0f };

    m_Particle2.SizeBegin = 0.5f, m_Particle.SizeEnd = 0.0f, m_Particle.SizeVariation = 0.3f;

    m_Particle2.LifeTime = 3.0f;

    // ---------------------------------------------------------------------------------- //

    m_Player.Color = { 1.0f, 1.0f, 1.0f, 1.0f };
    //m_Player.TextureID = m_Textures[2].getRendererID();
    m_Player.setElasticityCoefficient(1.0f);
    m_Player.setFrictionCoefficient(1.0f);

    m_Dynamic.Color = { 0.0f, 1.0f, 1.0f, 1.0f };
    m_Dynamic.setElasticityCoefficient(0.0f);
    m_Dynamic.setFrictionCoefficient(1.0f);

    m_Ground.Color = { 0.0f, 0.0f, 1.0f, 1.0f };
    m_Ground.setElasticityCoefficient(0.0f);
    m_Ground.setFrictionCoefficient(0.05f);

    m_Box.Color = { 1.0f, 0.0f, 0.0f, 1.0f };
    m_Box.setElasticityCoefficient(0.0f);
    m_Box.setFrictionCoefficient(0.1f);

    m_Ceiling.Color = { 0.0f, 1.0f, 1.0f, 1.0f };
    m_Ceiling.setFrictionCoefficient(0.9f);

    m_PhysicsSystem.addPhysicalObject(&m_Dynamic);
    m_PhysicsSystem.addPhysicalObject(&m_Player);
    m_PhysicsSystem.addPhysicalObject(&m_Ground);
    m_PhysicsSystem.addPhysicalObject(&m_Box);
    m_PhysicsSystem.addPhysicalObject(&m_Ceiling);
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
        m_Particle2.Position = { m_Player.getPosition().x,  m_Player.getPosition().y };

        for (int i = 0; i < 5; i++)
        {
            m_ParticleSystem.Emit(m_Particle);
            m_ParticleSystem.Emit(m_Particle2);
        }
        m_ParticleSystem.OnUpdate(ts);

        m_PhysicsSystem.onUpdate(ts);

        bool SurfaceContact =
           (m_PhysicsSystem.areColliding(&m_Player, &m_Ground) &&
            m_Ground.getCollisionOccurence(&m_Player) == CollisionOccurence::TOP) ||
           (m_PhysicsSystem.areColliding(&m_Player, &m_Box) &&
            m_Box.getCollisionOccurence(&m_Player) == CollisionOccurence::TOP) ||
           (m_PhysicsSystem.areColliding(&m_Player, &m_Dynamic) &&
            m_Dynamic.getCollisionOccurence(&m_Player) == CollisionOccurence::TOP);

        if (Elysium::Input::isKeyPressed(ELY_KEY_SPACE) && SurfaceContact)
        {
            m_Player.Impulse.y = 10.0f;
        }

        if (Elysium::Input::isKeyPressed(ELY_KEY_A))
        {
            if (SurfaceContact)
                m_Player.Impulse.x = -0.5f;
            else
                m_Player.Impulse.x = -0.1f;
        }
        else if (Elysium::Input::isKeyPressed(ELY_KEY_D))
        {
            if (SurfaceContact)
                m_Player.Impulse.x = 0.5f;
            else
                m_Player.Impulse.x = 0.1f;
        }

        ImGui::Begin("Statistics");
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