#include "SandboxLayer.h"

SandboxLayer::SandboxLayer(bool* runSandbox, unsigned int width, unsigned int height) : Layer("Sandbox"),
    m_RunSandbox(runSandbox),
    m_Camera(-m_Height * (float)(width / height), m_Height* (float)(width / height), 0.0f, m_Height),
    m_ParticleSystem(100, m_Camera),
    m_Player({ 20.0f, 10.0f }, { 2.0f, 2.0f }, 10.0f)
{
    Elysium::Renderer2D::Init();

    m_Textures.reserve(7);
    m_Textures.emplace_back("res/texture/meadow.png");
    m_Textures.emplace_back("res/texture/Vader.png");
    m_Textures.emplace_back("res/texture/alec.png");
    m_Textures.emplace_back("res/texture/Idle (1).png");
    m_Textures.emplace_back("res/texture/RPGpack_sheet_2X.png");
    m_Textures.emplace_back("res/texture/platformPack_tilesheet.png");
    m_Textures.emplace_back("res/texture/background.png");

    m_Background = m_Textures[6].getTextureData();
    m_Background.repeatTexture({ 15.0f, 1.0f });

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

    m_Particle2.SizeBegin = 0.15f, m_Particle.SizeEnd = 0.0f, m_Particle.SizeVariation = 0.05f;

    m_Particle2.LifeTime = 3.0f;

    // ---------------------------------------------------------------------------------- //

    m_Player.m_TextureData = m_Textures[3].getTextureData();

    m_GroundTexture = m_Textures[5].getTextureData();
    m_GroundTexture.subtextureCoordinates({ 0, 6 }, { 128, 128 });

    m_BoxTexture = m_Textures[5].getTextureData();
    m_BoxTexture.subtextureCoordinates({ 4, 1 }, { 128, 128 });

    m_BallTexture = m_Textures[5].getTextureData();
    m_BallTexture.subtextureCoordinates({ 1, 1 }, { 128, 128 });

    m_MoveableBox = e_PhysicsSystem.createPhysicalBody(Elysium::BodyType::DYNAMIC, "Box", 10.0f, { 10.0f, 15.0f }, { 2.0f, 2.0f });
    m_Ball = e_PhysicsSystem.createPhysicalBody(Elysium::BodyType::DYNAMIC, "Ball", 10.0f, { -2.0f, 10.0f }, { 2.0f, 2.0f });
    m_Ground = e_PhysicsSystem.createPhysicalBody(Elysium::BodyType::STATIC, "Ground", 10.0f, { 0.0f, 0.0f }, { 500.0f, 2.0f });
    m_Box = e_PhysicsSystem.createPhysicalBody(Elysium::BodyType::STATIC, "sBox", 10.0f, { 2.5f, 2.0f }, { 2.0f, 2.0f });

    m_Circle = e_PhysicsSystem.createPhysicalBody(Elysium::BodyType::DYNAMIC, "Ball", 10.0f, { 5.0f, 10.0f }, { 2.0f, 2.0f });

    Elysium::PhysicalBody& player = e_PhysicsSystem.getPhysicalBody(m_Player.getIdentifier());
    player.Radius = 1.0f;

    Elysium::PhysicalBody& ground = e_PhysicsSystem.getPhysicalBody(m_Ground);
    ground.setFrictionCoefficient(0.5f);
    //ground.setElasticityCoefficient(1.0f);

    Elysium::PhysicalBody& ball = e_PhysicsSystem.getPhysicalBody(m_Ball);
    ball.Radius = 1.0f;
    ball.setElasticityCoefficient(1.0f);

    Elysium::PhysicalBody& circle = e_PhysicsSystem.getPhysicalBody(m_Circle);
    circle.Radius = 1.0f;
}

SandboxLayer::~SandboxLayer()
{
    Elysium::Renderer2D::Shutdown();
}

void SandboxLayer::onUpdate(Elysium::Timestep ts)
{
    if (*m_RunSandbox)
    {
        Elysium::PhysicalBody& player = e_PhysicsSystem.getPhysicalBody(m_Player.getIdentifier());
        Elysium::PhysicalBody& box = e_PhysicsSystem.getPhysicalBody(m_MoveableBox);
        Elysium::PhysicalBody& ground = e_PhysicsSystem.getPhysicalBody(m_Ground);
        Elysium::PhysicalBody& sBox = e_PhysicsSystem.getPhysicalBody(m_Box);
        Elysium::PhysicalBody& ball = e_PhysicsSystem.getPhysicalBody(m_Ball);
        Elysium::PhysicalBody& circle = e_PhysicsSystem.getPhysicalBody(m_Circle);

        m_Camera.setPosition({ player.Position.x, player.Position.y - (player.getSize().y * 4.0f), 0.0f });

        Elysium::Renderer2D::beginScene(m_Camera);
        Elysium::Renderer2D::drawQuad({ 0.0f, 15.0f }, { 1000.0f, 30.0f }, m_Background);
        Elysium::Renderer2D::endScene();

        auto mousePosition = Elysium::Input::getMousePosition();
        auto width = Elysium::Application::Get().getWindow().getWidth();
        auto height = Elysium::Application::Get().getWindow().getHeight();

        m_Particle.Position = m_Camera.getScreenToWorldPosition(width, height, mousePosition);
        m_Particle2.Position = { player.Position.x, player.Position.y };

        for (int i = 0; i < 5; i++)
        {
            m_ParticleSystem.Emit(m_Particle);
            m_ParticleSystem.Emit(m_Particle2);
        }
        m_ParticleSystem.OnUpdate(ts);
        e_PhysicsSystem.onUpdate(ts);

        m_Player.onUpdate(ts);

        Elysium::Renderer2D::beginScene(m_Camera);
        Elysium::Renderer2D::drawQuad(player.Position, player.getSize(), m_Player.m_TextureData);
        Elysium::Renderer2D::drawQuad(box.Position, box.getSize(), m_BoxTexture);
        Elysium::Renderer2D::drawQuad(sBox.Position, sBox.getSize(), m_BoxTexture);
        Elysium::Renderer2D::drawQuad(ground.Position, ground.getSize(), m_GroundTexture);
        Elysium::Renderer2D::drawQuad(ball.Position, ball.getSize(), m_BallTexture);
        Elysium::Renderer2D::drawQuad(circle.Position, circle.getSize(), m_BallTexture);
        Elysium::Renderer2D::endScene();

        ImGui::Begin("Statistics");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Number of Draw Calls: %d", Elysium::Renderer2D::getStats().DrawCount);
        ImGui::Text("Number of Quads: %d", Elysium::Renderer2D::getStats().QuadCount);
        ImGui::End();

        Elysium::Renderer2D::resetStats();
    }
}

void SandboxLayer::onEvent(Elysium::Event& event)
{
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
    unsigned int width = event.getWidth();
    unsigned int height = event.getHeight();
    if (width > 0 && height > 0)
    m_Camera.setProjection(-m_Height * (float)(width / height), m_Height * (float)(width / height),
        0.0f, m_Height);
    return false;
}