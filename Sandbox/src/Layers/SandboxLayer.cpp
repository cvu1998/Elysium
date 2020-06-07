#include "SandboxLayer.h"

SandboxLayer::SandboxLayer(bool* runSandbox, unsigned int width, unsigned int height) : Layer("Sandbox"),
    m_RunSandbox(runSandbox),
    m_CameraController((float)width / (float)height, 3.0f),
    m_ParticleSystem(100, m_CameraController.getCamera()),
    m_PhysicsSystem(5.0f, m_CameraController.getCamera()),
    m_Player({ 0.0f, 10.0f }, { 1.0f, 2.0f }, 10.0f),
    m_Dynamic({ 5.0f, 15.0f }, { 2.0f, 2.0f }, 10.0f),
    m_Ground({ 0.0f, 0.0f }, { 1000.0f, 2.0f }),
    m_Box({ 2.5f, 2.0f }, { 2.0f, 2.0f })
{
    Elysium::Renderer2D::Init();

    m_Textures.reserve(7);
    m_Textures.emplace_back("res/texture/meadow.png");
    m_Textures.emplace_back("res/texture/Vader.png");
    m_Textures.emplace_back("res/texture/alec.png");
    m_Textures.emplace_back("res/texture/player-sprite.png");
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

    m_Particle2.SizeBegin = 0.5f, m_Particle.SizeEnd = 0.0f, m_Particle.SizeVariation = 0.3f;

    m_Particle2.LifeTime = 3.0f;

    // ---------------------------------------------------------------------------------- //

    m_Player.Color = { 1.0f, 1.0f, 1.0f, 1.0f };
    m_Player.TextureData = m_Textures[3].getTextureData();
    m_Player.TextureData.subtextureCoordinates({ 7.5, 2.5 }, { 64, 128 });
    m_Player.setElasticityCoefficient(0.0f);
    m_Player.setFrictionCoefficient(1.0f);

    m_Dynamic.TextureData = m_Textures[5].getTextureData();
    m_Dynamic.TextureData.subtextureCoordinates({ 4, 1 }, { 128, 128 });
    m_Dynamic.setElasticityCoefficient(0.0f);
    m_Dynamic.setFrictionCoefficient(1.0f);

    m_Ground.TextureData = m_Textures[5].getTextureData();
    m_Ground.TextureData.subtextureCoordinates({ 0, 6 }, { 128, 128 });
    m_Ground.setElasticityCoefficient(0.0f);
    m_Ground.setFrictionCoefficient(0.5f);

    m_Box.TextureData = m_Textures[5].getTextureData();
    m_Box.TextureData.subtextureCoordinates({ 4, 1 }, { 128, 128 });
    m_Box.setElasticityCoefficient(0.0f);
    m_Box.setFrictionCoefficient(1.0f);

    m_PhysicsSystem.addPhysicalObject(&m_Dynamic);
    m_PhysicsSystem.addPhysicalObject(&m_Player);
    m_PhysicsSystem.addPhysicalObject(&m_Ground);
    m_PhysicsSystem.addPhysicalObject(&m_Box);
}

SandboxLayer::~SandboxLayer()
{
    Elysium::Renderer2D::Shutdown();
}

void SandboxLayer::onUpdate(Elysium::Timestep ts)
{
    if (*m_RunSandbox)
    {
        m_CameraController.onUpdate(ts);

        Elysium::Renderer2D::beginScene(m_CameraController.getCamera());

        Elysium::Renderer2D::drawQuad({ 0.0f, 15.0f }, { 1000.0f, 30.0f }, m_Background);
        
        Elysium::Renderer2D::endScene();

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
            m_Player.Impulse.y = 5.0f * m_Player.getMass();
        }

        if (Elysium::Input::isKeyPressed(ELY_KEY_A))
        {
            if (m_PlayerLookingRight)
            {
                m_Player.TextureData.reflectAroundYAxis();
                m_PlayerLookingRight = false;
            }

            if (SurfaceContact)
            {
                m_Player.Impulse.x = -10.0f * m_Player.getMass() * ts;
            }
            else
                m_Player.Impulse.x = -2.0f * m_Player.getMass() * ts;
        }
        else if (Elysium::Input::isKeyPressed(ELY_KEY_D))
        {
            if (!m_PlayerLookingRight)
            {
                m_Player.TextureData.reflectAroundYAxis();
                m_PlayerLookingRight = true;
            }

            if (SurfaceContact)
            {
                m_Player.Impulse.x = 10.0f * m_Player.getMass() * ts;
            }
            else
                m_Player.Impulse.x = 2.0f * m_Player.getMass() * ts;
        }

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