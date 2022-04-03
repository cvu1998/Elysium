#include "PerformanceScene.h"

PerformanceScene::PerformanceScene(unsigned int width, unsigned int height) : Elysium::Scene("Stress Test"),
m_Camera(-m_Height * (float)(width / height), m_Height * (float)(width / height), -m_Height * 0.5f, m_Height * 0.5f),
m_ParticleSystem(17500, Elysium::UpdateDevice::CPU),
m_Player({ { -12.5f, 20.0f } })
{
    Elysium::Random::Init();

    m_Textures.reserve(11);
    m_Textures.emplace_back("res/texture/Idle (1).png");
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

    m_Background = m_Textures[2].getTextureData();

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

    m_Player.m_IdleTexture = m_Textures[0].getTextureData();
    m_Player.m_TextureData = m_Player.m_IdleTexture;

    for (size_t i = 3; i < m_Textures.size(); i++)
        m_Player.RunAnimation.Textures[i - 3] = m_Textures[i].getTextureData();

    m_GroundTexture = m_Textures[1].getTextureData();
    m_GroundTexture.subtextureCoordinates({ 0, 6 }, { 128, 128 });

    m_BoxTexture = m_Textures[1].getTextureData();
    m_BoxTexture.subtextureCoordinates({ 4, 1 }, { 128, 128 });

    e_PhysicsSystem2D.createPhysicalBody(&m_Ground, Elysium::BodyType::STATIC, Elysium::Collider::QUAD, "Ground", 10.0f, { 0.0f, 0.0f }, { 5000.0f, 2.0f });

    Elysium::PhysicalBody2D* ground = e_PhysicsSystem2D.getPhysicalBody(m_Ground);
    ground->setFrictionCoefficient(1.0f);

    float depth = -((float)m_GroundLayers.size() * 2.0f);
    for (size_t i = 0; i < m_GroundLayers.size(); i++)
    {
        e_PhysicsSystem2D.createPhysicalBody(&m_GroundLayers[i], Elysium::BodyType::STATIC, Elysium::Collider::QUAD, "Layer", 10.0f, { 0.0f, depth + (float)(2.0f * i) }, { 500.0f, 2.0f });
    }
}

PerformanceScene::~PerformanceScene()
{
    e_PhysicsSystem2D.clear();
}

void PerformanceScene::onUpdate(Elysium::Timestep ts)
{
    const Elysium::PhysicalBody2D* player = m_Player.getBody();
    const Elysium::PhysicalBody2D& ground = e_PhysicsSystem2D.readPhysicalBody(m_Ground);

    m_SpawnTime += ts;
    if (m_Index < m_Boxes.size() && m_SpawnTime > 0.05f)
    {
        float x = (Elysium::Random::Float() * 200.0f) - 100.0f;
        const float y = (float)m_Boxes.size() / 10.0f;
        Elysium::Vector2 position = { x, y };
        e_PhysicsSystem2D.createPhysicalBody(&m_Boxes[m_Index], Elysium::BodyType::DYNAMIC, Elysium::Collider::QUAD, "Box", 5.0f, position, { 2.0f, 2.0f });
        
        Elysium::PhysicalBody2D* box = e_PhysicsSystem2D.getPhysicalBody(m_Boxes[m_Index]);
        box->setFrictionCoefficient(1.0f);

        m_Index++;
        m_SpawnTime = 0.0f;
    }

    m_Camera.setPosition({ player->Position.x, player->Position.y + (player->getSize().y * 4.0f), 0.0f });

    Elysium::Renderer::beginScene(m_Camera);
    Elysium::Renderer::drawQuad({ 0.0f, 15.0f }, { 1000.0f, 30.0f }, m_Background, { 15.0f, 1.0f });
    Elysium::Renderer::endScene();

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
    m_ParticleSystem.onUpdate(ts);
    m_ParticleSystem.onRender(m_Camera);

    e_PhysicsSystem2D.onUpdate(ts);

    m_Player.onUpdate(ts);

    Elysium::Renderer::beginScene(m_Camera);
    Elysium::Renderer::drawQuad(player->Position, player->getSize(), m_Player.m_TextureData);
    Elysium::Renderer::drawQuad(ground.Position, ground.getSize(), m_GroundTexture);
    for (Elysium::BodyHandle& body : m_GroundLayers)
    {
        const Elysium::PhysicalBody2D& layer = e_PhysicsSystem2D.readPhysicalBody(body);
        Elysium::Renderer::drawQuad(layer.Position, layer.getSize(), m_GroundTexture);
    }
    unsigned int number = 0;
    for (size_t i = 0; i < m_Index; i++)
    {
        const Elysium::PhysicalBody2D& b = e_PhysicsSystem2D.readPhysicalBody(m_Boxes[i]);
        Elysium::Renderer::drawQuadWithRotation(b.Position, b.getSize(), b.Rotation, m_BoxTexture);
        if (b.Position.y > 0.0f)
            number++;
    }
    Elysium::Renderer::endScene();

    ImGui::Begin("Statistics");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Number of Draw Calls: %d", Elysium::Renderer::getStats().DrawCount);
    ImGui::Text("Number of Quads: %d", Elysium::Renderer::getStats().QuadCount);
    ImGui::Text("Number of Boxes: %d / %d", m_Index, m_Boxes.size());
    ImGui::Text("Number of Valid Boxes: %d / %d", number, m_Index);
    ImGui::End();

    Elysium::Renderer::resetStats();
}

void PerformanceScene::onEvent(Elysium::Event& event)
{
    Elysium::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Elysium::WindowResizeEvent>(BIND_EVENT_FUNCTION(PerformanceScene::onWindowResizeEvent));

    m_Player.onEvent(event);
}

bool PerformanceScene::onWindowResizeEvent(Elysium::WindowResizeEvent& event)
{
    unsigned int width = event.getWidth();
    unsigned int height = event.getHeight();
    if (width > 0 && height > 0)
        m_Camera.setProjection(-m_Height * (float)(width / height), m_Height * (float)(width / height),
            -m_Height * 0.5f, m_Height * 0.5f);
    return false;
}