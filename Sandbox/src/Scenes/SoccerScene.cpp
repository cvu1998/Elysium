#include "SoccerScene.h"

SoccerScene::SoccerScene(unsigned int width, unsigned int height) :
Scene("Soccer"),
m_Camera(-m_Height * (float)(width / height), m_Height* (float)(width / height), 0.0f, m_Height),
m_Player({ -27.5f, 20.0f }),
m_Adversary({ 27.5f, 20.0f }, ELY_KEY_UP, ELY_KEY_LEFT, ELY_KEY_RIGHT,
    ELY_KEY_DOWN, ELY_KEY_RIGHT_SHIFT, ELY_KEY_SLASH)
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

    // ---------------------------------------------------------------------------------- //

    m_Player.m_IdleTexture = m_Textures[0].getTextureData();
    m_Player.m_TextureData = m_Player.m_IdleTexture;

    for (size_t i = 4; i < m_Textures.size(); i++)
        m_Player.RunAnimation.Textures[i - 4] = m_Textures[i].getTextureData();


    m_Adversary.m_IdleTexture = m_Textures[0].getTextureData();
    m_Adversary.m_TextureData = m_Adversary.m_IdleTexture;

    for (size_t i = 4; i < m_Textures.size(); i++)
        m_Adversary.RunAnimation.Textures[i - 4] = m_Textures[i].getTextureData();

    m_GroundTexture = m_Textures[1].getTextureData();
    m_GroundTexture.subtextureCoordinates({ 0, 6 }, { 128, 128 });

    m_BallTexture = m_Textures[3].getTextureData();

    Elysium::PhysicsSystem2D& physicsSys = Elysium::PhysicsSystem2D::Get();
    m_Ground = physicsSys.createPhysicalBody(
        Elysium::BodyType::STATIC,
        Elysium::Collider::QUAD,
        "Ground", 10.0f,
        { 0.0f, 0.0f },
        { 500.0f, 2.0f }
    );
    m_Ball = physicsSys.createPhysicalBody(
        Elysium::BodyType::DYNAMIC,
        Elysium::Collider::CIRCLE,
        "Ball",
        2.5f,
        { 0.0f, 10.0f },
        { 2.0f, 2.0f }
    );

    m_Rectangles[0] = physicsSys.createPhysicalBody(Elysium::BodyType::STATIC, Elysium::Collider::QUAD, "Rectangle", 0.0f, { 0.0f, 29.0f }, { 60.0f, 2.0f });
    m_Rectangles[1] = physicsSys.createPhysicalBody(Elysium::BodyType::STATIC, Elysium::Collider::QUAD, "Rectangle", 0.0f, { -30.0f, 20.0f }, { 2.0f, 30.0f });
    m_Rectangles[2] = physicsSys.createPhysicalBody(Elysium::BodyType::STATIC, Elysium::Collider::QUAD, "Rectangle", 0.0f, { 30.0f, 20.0f }, { 2.0f, 30.0f });

    physicsSys.getBody(m_Ground).setFrictionCoefficient(1.0f);

    m_Player.setBallID(m_Ball);
    m_Adversary.setBallID(m_Ball);
    physicsSys.getBody(m_Ball).setElasticityCoefficient(1.0f);

    m_Camera.setPosition({ 0.0f, -1.0f, 0.0f });
}

SoccerScene::~SoccerScene()
{
}

void SoccerScene::onUpdate(Elysium::Timestep ts)
{
    Elysium::PhysicsSystem2D& physicsSys = Elysium::PhysicsSystem2D::Get();
    if (m_GameOver)
    {
        physicsSys.removePhysicalBody(m_Ball);

        m_Adversary.getBody().Position = { 27.5f, 20.0f };
        m_Player.getBody().Position = { -27.5f, 20.0f };

        m_Ball = physicsSys.createPhysicalBody(
            Elysium::BodyType::DYNAMIC,
            Elysium::Collider::CIRCLE,
            "Ball",
            1.0f,
            { 0.0f, 10.0f },
            { 2.0f, 2.0f }
        );
          
        m_Player.setBallID(m_Ball);
        m_Adversary.setBallID(m_Ball);
        physicsSys.getBody(m_Ball).setElasticityCoefficient(1.0f);

        m_GameOver = false;
    }

    Elysium::Renderer::beginScene(m_Camera);
    Elysium::Renderer::drawQuad({ 0.0f, 15.0f }, { 1000.0f, 30.0f }, m_Background, { 15.0f, 1.0f });
    Elysium::Renderer::endScene();

    auto mousePosition = Elysium::Input::getMousePosition();
    auto width = Elysium::Application::Get().getWindow().getWidth();
    auto height = Elysium::Application::Get().getWindow().getHeight();

    physicsSys.onUpdate(ts);

    m_Adversary.onUpdate(ts);
    m_Player.onUpdate(ts);

    if (physicsSys.getBody(m_Ball).Position.x > 30.0f)
    {
        m_BlueScore++;
        m_GameOver = true;
    }
    else if (physicsSys.getBody(m_Ball).Position.x < -30.0f)
    {
        m_RedScore++;
        m_GameOver = true;
    }

    Elysium::Renderer::beginScene(m_Camera);
    Elysium::Renderer::drawQuad(
        m_Adversary.getBody().Position,
        m_Adversary.getBody().getSize(),
        m_Adversary.m_TextureData,
        { 1.0f, 1.0f },
        { 1.0f, 0.0f, 0.0f, 1.0f }
    );
    Elysium::Renderer::drawQuad(
        m_Player.getBody().Position,
        m_Player.getBody().getSize(),
        m_Player.m_TextureData,
        { 1.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f, 1.0f }
    );
    Elysium::Renderer::drawQuadWithRotation(
        physicsSys.getBody(m_Ground).Position,
        physicsSys.getBody(m_Ground).getSize(),
        physicsSys.getBody(m_Ground).Rotation,
        m_GroundTexture
    );
    Elysium::Renderer::drawQuadWithRotation(
        physicsSys.getBody(m_Ball).Position,
        physicsSys.getBody(m_Ball).getSize(),
        physicsSys.getBody(m_Ball).Rotation,
        m_BallTexture
    );

    constexpr Elysium::Vector4 colors[3] = {
        { 0.0f, 1.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f, 1.0f },
        { 1.0f, 0.0f, 0.0f, 1.0f }
    };

    for (size_t i = 0 ; i < m_Rectangles.size(); i++)
    {
        Elysium::Renderer::drawQuad(
            physicsSys.getBody(m_Rectangles[i]).Position,
            physicsSys.getBody(m_Rectangles[i]).getSize(),
            colors[i]
        );
    }
    Elysium::Renderer::endScene();

    ImGui::Begin("Statistics");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Number of Draw Calls: %d", Elysium::Renderer::getStats().DrawCount);
    ImGui::Text("Number of Quads: %d", Elysium::Renderer::getStats().QuadCount);
    ImGui::Text("Red: %d : Blue %d", m_RedScore, m_BlueScore);
    ImGui::End();

    Elysium::Renderer::resetStats();
}

void SoccerScene::onEvent(Elysium::Event& event)
{
    Elysium::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Elysium::KeyPressedEvent>(BIND_EVENT_FUNCTION(SoccerScene::onKeyPressedEvent));
    dispatcher.Dispatch<Elysium::WindowResizeEvent>(BIND_EVENT_FUNCTION(SoccerScene::onWindowResizeEvent));

    m_Player.onEvent(event);
    m_Adversary.onEvent(event);
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