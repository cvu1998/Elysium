#include "PerformanceLayer.h"

PerformanceLayer::PerformanceLayer(bool* runGame, unsigned int width, unsigned int height) : Layer("Stress Test"),
m_RunGame(runGame),
m_Camera(-m_Height * (float)(width / height), m_Height * (float)(width / height), 0.0f, m_Height),
m_ParticleSystem(100, m_Camera)
{
    m_Textures.reserve(15);
    m_Textures.emplace_back("res/texture/meadow.png");
    m_Textures.emplace_back("res/texture/Vader.png");
    m_Textures.emplace_back("res/texture/alec.png");
    m_Textures.emplace_back("res/texture/Idle (1).png");
    m_Textures.emplace_back("res/texture/RPGpack_sheet_2X.png");
    m_Textures.emplace_back("res/texture/platformPack_tilesheet.png");
    m_Textures.emplace_back("res/texture/background.png");

    m_Textures.emplace_back("res/texture/Run (1).png");
    m_Textures.emplace_back("res/texture/Run (2).png");
    m_Textures.emplace_back("res/texture/Run (3).png");
    m_Textures.emplace_back("res/texture/Run (4).png");
    m_Textures.emplace_back("res/texture/Run (5).png");
    m_Textures.emplace_back("res/texture/Run (6).png");
    m_Textures.emplace_back("res/texture/Run (7).png");
    m_Textures.emplace_back("res/texture/Run (8).png");

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

    m_Player.m_IdleTexture = m_Textures[3].getTextureData();
    m_Player.m_TextureData = m_Player.m_IdleTexture;

    for (size_t i = 7; i < m_Textures.size(); i++)
        m_Player.m_RunAnimation.Textures[i - 7] = m_Textures[i].getTextureData();

    m_GroundTexture = m_Textures[5].getTextureData();
    m_GroundTexture.subtextureCoordinates({ 0, 6 }, { 128, 128 });

    m_BoxTexture = m_Textures[5].getTextureData();
    m_BoxTexture.subtextureCoordinates({ 4, 1 }, { 128, 128 });

    m_BallTexture = m_Textures[5].getTextureData();
    m_BallTexture.subtextureCoordinates({ 10, 6 }, { 128, 128 });

    e_PhysicsSystem.createPhysicalBody(&m_Ground, Elysium::BodyType::STATIC, "Ground", 10.0f, { 0.0f, 0.0f }, { 5000.0f, 2.0f });

    float depth = -((float)m_GroundLayers.size() * 2.0f);
    for (size_t i = 0; i < m_GroundLayers.size(); i++)
    {
        e_PhysicsSystem.createPhysicalBody(&m_GroundLayers[i], Elysium::BodyType::STATIC, "Layer", 10.0f, { 0.0f, depth + (float)(2.0f * i) }, { 500.0f, 2.0f });
    }
}

void PerformanceLayer::onUpdate(Elysium::Timestep ts)
{
    if (*m_RunGame)
    {
        const Elysium::PhysicalBody* player = m_Player.getBody();
        const Elysium::PhysicalBody& ground = e_PhysicsSystem.readPhysicalBody(m_Ground);

        m_SpawnTime += ts;
        if (m_Index < m_Boxes.size() && m_SpawnTime > 0.05f)
        {
            float x = (Random::Float() * 200.0f) - 100.0f;
            Elysium::Vector2 position = { x, 20.0f };
            e_PhysicsSystem.createPhysicalBody(&m_Boxes[m_Index], Elysium::BodyType::DYNAMIC, "Box", 5.0f, position, { 2.0f, 2.0f });

            m_Index++;
            m_SpawnTime = 0.0f;
        }

        m_Camera.setPosition({ player->Position.x, player->Position.y - (player->getSize().y * 4.0f), 0.0f });

        Elysium::Renderer2D::beginScene(m_Camera);
        Elysium::Renderer2D::drawQuad({ 0.0f, 15.0f }, { 1000.0f, 30.0f }, m_Background);
        Elysium::Renderer2D::endScene();

        auto mousePosition = Elysium::Input::getMousePosition();
        auto width = Elysium::Application::Get().getWindow().getWidth();
        auto height = Elysium::Application::Get().getWindow().getHeight();

        m_Particle.Position = m_Camera.getScreenToWorldPosition(width, height, mousePosition);
        m_Particle2.Position = { player->Position.x, player->Position.y };

        for (int i = 0; i < 5; i++)
        {
            m_ParticleSystem.Emit(m_Particle);
            m_ParticleSystem.Emit(m_Particle2);
        }
        m_ParticleSystem.OnUpdate(ts);
        e_PhysicsSystem.onUpdate(ts);

        m_Player.onUpdate(ts);

        Elysium::Renderer2D::beginScene(m_Camera);
        Elysium::Renderer2D::drawQuad(player->Position, player->getSize(), m_Player.m_TextureData);
        Elysium::Renderer2D::drawQuad(ground.Position, ground.getSize(), m_GroundTexture);
        for (Elysium::BodyHandle& body : m_GroundLayers)
        {
            const Elysium::PhysicalBody& layer = e_PhysicsSystem.readPhysicalBody(body);
            Elysium::Renderer2D::drawQuad(layer.Position, layer.getSize(), m_GroundTexture);
        }
        unsigned int number = 0;
        for (size_t i = 0; i < m_Index; i++)
        {
            const Elysium::PhysicalBody& b = e_PhysicsSystem.readPhysicalBody(m_Boxes[i]);
            Elysium::Renderer2D::drawQuadWithRotation(b.Position, b.getSize(), b.Rotation, m_BoxTexture);
            if (b.Position.y > 0.0f)
                number++;
        }
        Elysium::Renderer2D::endScene();

        ImGui::Begin("Statistics");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Number of Draw Calls: %d", Elysium::Renderer2D::getStats().DrawCount);
        ImGui::Text("Number of Quads: %d", Elysium::Renderer2D::getStats().QuadCount);
        ImGui::Text("Number of Boxes: %d / %d", m_Index, m_Boxes.size());
        ImGui::Text("Number of Valid Boxes: %d / %d", number, m_Index);
        ImGui::End();

        Elysium::Renderer2D::resetStats();
    }
}

void PerformanceLayer::onEvent(Elysium::Event& event)
{
    Elysium::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Elysium::WindowResizeEvent>(BIND_EVENT_FUNCTION(PerformanceLayer::onWindowResizeEvent));
}

bool PerformanceLayer::onWindowResizeEvent(Elysium::WindowResizeEvent& event)
{
    unsigned int width = event.getWidth();
    unsigned int height = event.getHeight();
    if (width > 0 && height > 0)
        m_Camera.setProjection(-m_Height * (float)(width / height), m_Height * (float)(width / height),
            0.0f, m_Height);
    return false;
}