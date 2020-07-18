#include "GameLayer.h"

GameLayer::GameLayer(bool* runGame, unsigned int width, unsigned int height) : Layer("Sandbox"),
m_RunGame(runGame),
m_Camera(-m_Height * (float)(width / height), m_Height* (float)(width / height), 0.0f, m_Height),
m_ParticleSystem(100, m_Camera),
m_PhysicsSystem(10.0f, m_Camera),
m_Player(m_PhysicsSystem, { 20.0f, 10.0f }, { 1.0f, 2.0f }, 10.0f)
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

    m_Particle2.SizeBegin = 0.15f, m_Particle.SizeEnd = 0.0f, m_Particle.SizeVariation = 0.05f;

    m_Particle2.LifeTime = 3.0f;

    // ---------------------------------------------------------------------------------- //

    m_BoxTexture = m_Textures[5].getTextureData();
    m_BoxTexture.subtextureCoordinates({ 4, 1 }, { 128, 128 });

    m_MoveableBox = m_PhysicsSystem.createPhysicalBody(Elysium::BodyType::DYNAMIC, "Box", 10.0f, { 10.0f, 15.0f }, { 2.0f, 2.0f });
    m_Ground = m_PhysicsSystem.createPhysicalBody(Elysium::BodyType::STATIC, "Ground", 10.0f, { 0.0f, 0.0f }, { 500.0f, 2.0f });
    m_Box = m_PhysicsSystem.createPhysicalBody(Elysium::BodyType::STATIC, "sBox", 10.0f, { 2.5f, 2.0f }, { 2.0f, 2.0f });

    m_Player.m_TextureData = m_Textures[3].getTextureData();
    m_Player.m_TextureData.subtextureCoordinates({ 7.5, 2.5 }, { 64, 128 });

    m_GroundTexture = m_Textures[5].getTextureData();
    m_GroundTexture.subtextureCoordinates({ 0, 6 }, { 128, 128 });

    float depth = -((float)m_GroundLayers.size() * 2.0f);
    for (size_t i = 0; i < m_GroundLayers.size(); i++)
    {
        m_GroundLayers[i] = m_PhysicsSystem.createPhysicalBody(Elysium::BodyType::STATIC, "Layer", 10.0f, { 0.0f, depth + (float)(2.0f * i) }, { 2.0f, 2.0f });
    }
}

GameLayer::~GameLayer()
{
    Elysium::Renderer2D::Shutdown();
}

void GameLayer::onUpdate(Elysium::Timestep ts)
{
    if (*m_RunGame)
    {
        Elysium::PhysicalBody& player = m_PhysicsSystem.getPhysicalBody(m_Player.getIdentifier());
        Elysium::PhysicalBody& box = m_PhysicsSystem.getPhysicalBody(m_MoveableBox);
        Elysium::PhysicalBody& ground = m_PhysicsSystem.getPhysicalBody(m_Ground);
        Elysium::PhysicalBody& sBox = m_PhysicsSystem.getPhysicalBody(m_Box);

        m_SpawnTime += ts;
        if (m_Index < m_Boxes.size() && m_SpawnTime > 0.5f)
        {
            float x = (Random::Float() * 50.0f) - 25.0f;
            Elysium::Vector2 position = { x, 50.0f};
            m_Boxes[m_Index] = m_PhysicsSystem.createPhysicalBody(Elysium::BodyType::DYNAMIC, "Box", 10.0f, position, { 2.0f, 2.0f });
        
            m_Index++;
            m_SpawnTime = 0.0f;
        }

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
        m_PhysicsSystem.onUpdate(ts);

        if (Elysium::Input::isKeyPressed(ELY_KEY_A))
        {
            if (m_Player.m_PlayerLookingRight)
            {
                m_Player.m_TextureData.reflectAroundYAxis();
                m_Player.m_PlayerLookingRight = false;
            }
            player.Impulse.x = -1.0f * player.getMass() * ts;
        }
        else if (Elysium::Input::isKeyPressed(ELY_KEY_D))
        {
            if (!m_Player.m_PlayerLookingRight)
            {
                m_Player.m_TextureData.reflectAroundYAxis();
                m_Player.m_PlayerLookingRight = true;
            }
            player.Impulse.x = 1.0f * player.getMass() * ts;
        }

        Elysium::Renderer2D::beginScene(m_Camera);
        Elysium::Renderer2D::drawQuad(player.Position, player.getSize(), m_Player.m_TextureData);
        Elysium::Renderer2D::drawQuad(box.Position, box.getSize(), m_BoxTexture);
        Elysium::Renderer2D::drawQuad(sBox.Position, sBox.getSize(), m_BoxTexture);
        Elysium::Renderer2D::drawQuad(ground.Position, ground.getSize(), m_GroundTexture);
        for (Elysium::PhysicsBody& body : m_GroundLayers)
        {
            Elysium::PhysicalBody& layer = m_PhysicsSystem.getPhysicalBody(body);
            Elysium::Renderer2D::drawQuad(layer.Position, layer.getSize(), m_GroundTexture);
        }
        for (size_t i = 0; i < m_Index; i++)
        {
            Elysium::PhysicalBody& b = m_PhysicsSystem.getPhysicalBody(m_Boxes[i]);
            Elysium::Renderer2D::drawQuad(b.Position, b.getSize(), m_BoxTexture);
        }
        Elysium::Renderer2D::endScene();

        ImGui::Begin("Statistics");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Number of Draw Calls: %d", Elysium::Renderer2D::getStats().DrawCount);
        ImGui::Text("Number of Quads: %d", Elysium::Renderer2D::getStats().QuadCount);
        ImGui::Text("Number of Boxes: %d / %d", m_Index, m_Boxes.size());
        ImGui::End();

        Elysium::Renderer2D::resetStats();
    }
}

void GameLayer::onEvent(Elysium::Event& event)
{
    Elysium::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Elysium::KeyPressedEvent>(BIND_EVENT_FUNCTION(GameLayer::onKeyPressedEvent));
    dispatcher.Dispatch<Elysium::WindowResizeEvent>(BIND_EVENT_FUNCTION(GameLayer::onWindowResizeEvent));
}

bool GameLayer::onKeyPressedEvent(Elysium::KeyPressedEvent& event)
{
    return false;
}

bool GameLayer::onWindowResizeEvent(Elysium::WindowResizeEvent& event)
{
    unsigned int width = event.getWidth();
    unsigned int height = event.getHeight();
    if (width > 0 && height > 0)
        m_Camera.setProjection(-m_Height * (float)(width / height), m_Height * (float)(width / height),
            0.0f, m_Height);
    return false;
}