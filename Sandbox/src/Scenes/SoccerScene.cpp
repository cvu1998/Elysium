#include "SoccerScene.h"

SoccerScene::SoccerScene(unsigned int width, unsigned int height) : Scene("Soccer"),
m_Camera(-m_Height * (float)(width / height), m_Height* (float)(width / height), 0.0f, m_Height)
{
    m_Textures.reserve(12);
    m_Textures.emplace_back("res/texture/Idle (1).png");
    m_Textures.emplace_back("res/texture/platformPack_tilesheet.png");
    m_Textures.emplace_back("res/texture/background.png");
    m_Textures.emplace_back("res/texture/soccer_ball.png");

    m_Textures.emplace_back("res/texture/Run (1).png");
    m_Textures.emplace_back("res/texture/Run (2).png");
    m_Textures.emplace_back("res/texture/Run (3).png");
    m_Textures.emplace_back("res/texture/Run (4).png");
    m_Textures.emplace_back("res/texture/Run (5).png");
    m_Textures.emplace_back("res/texture/Run (6).png");
    m_Textures.emplace_back("res/texture/Run (7).png");
    m_Textures.emplace_back("res/texture/Run (8).png");

    m_Background = m_Textures[2].getTextureData();
    m_Background.repeatTexture({ 15.0f, 1.0f });
    // ---------------------------------------------------------------------------------- //

    m_Player.m_IdleTexture = m_Textures[0].getTextureData();
    m_Player.m_TextureData = m_Player.m_IdleTexture;

    for (size_t i = 4; i < m_Textures.size(); i++)
        m_Player.m_RunAnimation.Textures[i - 4] = m_Textures[i].getTextureData();


    m_Adversary.m_IdleTexture = m_Textures[0].getTextureData();
    m_Adversary.m_TextureData = m_Adversary.m_IdleTexture;

    for (size_t i = 4; i < m_Textures.size(); i++)
        m_Adversary.m_RunAnimation.Textures[i - 4] = m_Textures[i].getTextureData();

    m_GroundTexture = m_Textures[1].getTextureData();
    m_GroundTexture.subtextureCoordinates({ 0, 6 }, { 128, 128 });

    m_BallTexture = m_Textures[3].getTextureData();

    e_PhysicsSystem.createPhysicalBody(&m_Ground, Elysium::BodyType::STATIC, "Ground", 10.0f, { 0.0f, 0.0f }, { 500.0f, 2.0f });
    e_PhysicsSystem.createPhysicalBody(&m_Ball, Elysium::BodyType::DYNAMIC, "Ball", 1.0f, { 0.0f, 10.0f }, { 2.0f, 2.0f });

    e_PhysicsSystem.createPhysicalBody(&m_Rectangles[0], Elysium::BodyType::STATIC, "Rectangle", 0.0f, { 0.0f, 29.0f }, { 60.0f, 2.0f });
    e_PhysicsSystem.createPhysicalBody(&m_Rectangles[1], Elysium::BodyType::STATIC, "Rectangle", 0.0f, { -30.0f, 20.0f }, { 2.0f, 30.0f });
    e_PhysicsSystem.createPhysicalBody(&m_Rectangles[2], Elysium::BodyType::STATIC, "Rectangle", 0.0f, { 30.0f, 20.0f }, { 2.0f, 30.0f });

    Elysium::PhysicalBody* ground = e_PhysicsSystem.getPhysicalBody(m_Ground);
    ground->setFrictionCoefficient(0.75f);
    //ground.setElasticityCoefficient(1.0f);

    //ground.Rotation = Elysium::radians(10.0f);

    Elysium::PhysicalBody* ball = e_PhysicsSystem.getPhysicalBody(m_Ball);
    ball->setRadius(1.0f);
    ball->setElasticityCoefficient(1.0f);

    m_Camera.setPosition({ 0.0f, -1.0f, 0.0f });
}

SoccerScene::~SoccerScene()
{
    e_PhysicsSystem.clear();
}

void SoccerScene::onUpdate(Elysium::Timestep ts)
{
    if (m_GameOver)
    {
        e_PhysicsSystem.removePhysicalBody(m_Ball);
        Elysium::PhysicalBody* adversary = m_Adversary.getBody();
        Elysium::PhysicalBody* player = m_Player.getBody();

        adversary->Position = { 27.5f, 20.0f };
        player->Position = { -27.5f, 20.0f };

        e_PhysicsSystem.createPhysicalBody(&m_Ball, Elysium::BodyType::DYNAMIC, "Ball", 1.0f, { 0.0f, 10.0f }, { 2.0f, 2.0f });
            
        Elysium::PhysicalBody* ball = e_PhysicsSystem.getPhysicalBody(m_Ball);
        ball->setRadius(1.0f);
        ball->setElasticityCoefficient(1.0f);

        m_GameOver = false;
    }

    Elysium::PhysicalBody* adversary = m_Adversary.getBody();
    Elysium::PhysicalBody* player = m_Player.getBody();
    const Elysium::PhysicalBody& ground = e_PhysicsSystem.readPhysicalBody(m_Ground);
    const Elysium::PhysicalBody& ball = e_PhysicsSystem.readPhysicalBody(m_Ball);

    Elysium::Renderer2D::beginScene(m_Camera);
    Elysium::Renderer2D::drawQuad({ 0.0f, 15.0f }, { 1000.0f, 30.0f }, m_Background);
    Elysium::Renderer2D::endScene();

    auto mousePosition = Elysium::Input::getMousePosition();
    auto width = Elysium::Application::Get().getWindow().getWidth();
    auto height = Elysium::Application::Get().getWindow().getHeight();

    e_PhysicsSystem.onUpdate(ts);

    m_Adversary.onUpdate(ts);
    m_Adversary.kickBall(m_Ball);
    m_Adversary.movePlayer(player);
    m_Player.onUpdate(ts);
    m_Player.kickBall(m_Ball);
    m_Player.movePlayer(adversary);

    if (ball.Position.x > 30.0f)
    {
        m_BlueScore++;
        m_GameOver = true;
    }
    else if (ball.Position.x < -30.0f)
    {
        m_RedScore++;
        m_GameOver = true;
    }

    Elysium::Renderer2D::beginScene(m_Camera);
    Elysium::Renderer2D::drawQuad(adversary->Position, adversary->getSize(), m_Adversary.m_TextureData, { 1.0f, 0.0f, 0.0f, 1.0f });
    Elysium::Renderer2D::drawQuad(player->Position, player->getSize(), m_Player.m_TextureData, { 0.0f, 0.0f, 1.0f, 1.0f });
    Elysium::Renderer2D::drawQuadWithRotation(ground.Position, ground.getSize(), ground.Rotation, m_GroundTexture);
    Elysium::Renderer2D::drawQuadWithRotation(ball.Position, ball.getSize(), ball.Rotation, m_BallTexture);

    for (size_t i = 0 ; i < m_Rectangles.size(); i++)
    {
        const Elysium::PhysicalBody& rectangle = e_PhysicsSystem.readPhysicalBody(m_Rectangles[i]);
        switch (i)
        {
        case 1:
            Elysium::Renderer2D::drawQuad(rectangle.Position, rectangle.getSize(), { 0.0f, 0.0f, 1.0f, 1.0f });
            break;
        case 2:
            Elysium::Renderer2D::drawQuad(rectangle.Position, rectangle.getSize(), { 1.0f, 0.0f, 0.0f, 1.0f });
            break;
        default:
            Elysium::Renderer2D::drawQuad(rectangle.Position, rectangle.getSize(), { 0.0f, 1.0f, 0.0f, 1.0f });
        }
    }
    Elysium::Renderer2D::endScene();

    ImGui::Begin("Statistics");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Number of Draw Calls: %d", Elysium::Renderer2D::getStats().DrawCount);
    ImGui::Text("Number of Quads: %d", Elysium::Renderer2D::getStats().QuadCount);
    ImGui::Text("Red: %d : Blue %d", m_RedScore, m_BlueScore);
    ImGui::End();

    Elysium::Renderer2D::resetStats();
}

void SoccerScene::onEvent(Elysium::Event& event)
{
    Elysium::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Elysium::KeyPressedEvent>(BIND_EVENT_FUNCTION(SoccerScene::onKeyPressedEvent));
    dispatcher.Dispatch<Elysium::WindowResizeEvent>(BIND_EVENT_FUNCTION(SoccerScene::onWindowResizeEvent));
}

bool SoccerScene::onKeyPressedEvent(Elysium::KeyPressedEvent& event)
{
    return false;
}

bool SoccerScene::onWindowResizeEvent(Elysium::WindowResizeEvent& event)
{
    unsigned int width = event.getWidth();
    unsigned int height = event.getHeight();
    if (width > 0 && height > 0)
        m_Camera.setProjection(-m_Height * (float)(width / height), m_Height * (float)(width / height),
            0.0f, m_Height);
    return false;
}