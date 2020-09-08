#include "FlappyBirdScene.h"

FlappyBirdScene::FlappyBirdScene(unsigned int width, unsigned int height) :
    Elysium::Scene("Flappy Bird"),
    m_Camera(-m_Height * (float)(width / height), m_Height* (float)(width / height), -m_Height * 0.5f, m_Height * 0.5f),
    m_SpriteSheet("res/texture/Flappy-Bird-Sprite.png")
{
    std::vector<Elysium::Quad> quads;
    Elysium::Editor::getSceneFromFile("res/scenes/Flappy.ely", quads);

    for (Elysium::Quad quad : quads)
    {
        if (quad.Tag == "Bird")
        {
            m_InitialPosition = { quad.Position[0], quad.Position[1] };

            m_Bird.Body = e_PhysicsSystem.createPhysicalBody(Elysium::BodyType::DYNAMIC, "Bird", 2.0f,
                m_InitialPosition, { quad.Size[0], quad.Size[1] }, FlappyBird::onCollision);
            m_Bird.Body->AllowRotation = false;
            m_Bird.Body->setNumberOfCallbackExecution(1);

            m_Bird.Body->Impulse.x += m_InitialImpulse * m_Bird.Body->getMass();
        }
        else if (quad.Tag == "Ground")
        {
            m_Ground = e_PhysicsSystem.createPhysicalBody(Elysium::BodyType::STATIC, "Ground", 0.0f,
                { quad.Position[0], quad.Position[1] }, { quad.Size[0], quad.Size[1] });
            m_Ceiling = e_PhysicsSystem.createPhysicalBody(Elysium::BodyType::STATIC, "Ceiling", 0.0f,
                { quad.Position[0], quad.Position[1] }, { quad.Size[0], quad.Size[1] });
        }
        else if (quad.Tag == "Ceiling")
        {
            m_Ceiling = e_PhysicsSystem.createPhysicalBody(Elysium::BodyType::STATIC, "Ceiling", 0.0f,
                { quad.Position[0], quad.Position[1] }, { quad.Size[0], quad.Size[1] });
        }
    }
    m_SavedPosition = m_Bird.Body->Position.x - m_PipeDistance;
    m_Camera.setPosition({ m_Bird.Body->Position.x , m_Bird.Body->Position.y, 0.0 });

    m_BirdSprite = m_SpriteSheet.getTextureData();
    m_BirdSprite.subtextureCoordinates({ 3, 0 }, { 28, 28 });

    m_GroundSprite = m_SpriteSheet.getTextureData();
    m_GroundSprite.subtextureCoordinates({ 0, 0 }, { 168, 56 }, { 288, 456 });

    m_Background = m_SpriteSheet.getTextureData();
    m_Background.subtextureCoordinates({ 0, 1 }, { 144, 256 });

    m_BackgroundPositions.push_back(-45.0f);
    m_BackgroundPositions.push_back(-22.5f);
    m_BackgroundPositions.push_back(0);
    m_BackgroundPositions.push_back(22.5f);
    m_BackgroundPositions.push_back(45.0f);
    m_BackgroundPosition = 45.0f;

    m_LowerSprite = m_SpriteSheet.getTextureData();
    m_LowerSprite.subtextureCoordinates({ 3, 0 }, { 28, 168 }, { 0, 28 });
    m_UpperSprite = m_SpriteSheet.getTextureData();
    m_UpperSprite.subtextureCoordinates({ 2, 0 }, { 28, 168 }, { 0, 28 });
}

FlappyBirdScene::~FlappyBirdScene()
{
    e_PhysicsSystem.clear();
}

void FlappyBirdScene::onUpdate(Elysium::Timestep ts)
{
    if (m_Bird.PauseBird)
    {
        if (m_Cooldown >= 0.0f)
        {
            if (!m_GameOver)
            {
                m_Cooldown = -2.0f;
                m_GameOver = true;

                m_Bird.Body->AllowRotation = true;
            }
            else
            {
                m_Cooldown = 0.0f;
                m_GameOver = false;

                m_Bird.Body->Position = m_InitialPosition;
                m_Bird.PauseBird = false;
                m_Bird.Body->AllowRotation = false;

                m_Bird.Body->resetValues();
                m_Bird.Body->Impulse.x += m_InitialImpulse * m_Bird.Body->getMass();

                for (Elysium::BodyHandle handle : m_LowerPipes)
                {
                    e_PhysicsSystem.removePhysicalBody(handle);
                }
                for (Elysium::BodyHandle handle : m_UpperPipes)
                {
                    e_PhysicsSystem.removePhysicalBody(handle);
                }
                m_LowerPipes.clear();
                m_UpperPipes.clear();
                m_SavedPosition = m_Bird.Body->Position.x - m_PipeDistance;

                m_BackgroundPositions.clear();
                m_BackgroundPositions.push_back(-45.0f);
                m_BackgroundPositions.push_back(-22.5f);
                m_BackgroundPositions.push_back(0);
                m_BackgroundPositions.push_back(22.5f);
                m_BackgroundPositions.push_back(45.0f);
                m_BackgroundPosition = 45.0f;

                m_PipeIndex = 0;
            }
        }
        m_Cooldown += ts;
    }
    else
    {
        m_Bird.onUpdate(ts);
    }
    e_PhysicsSystem.onUpdate(ts);

    if (m_Bird.Body->Position.x - m_SavedPosition > m_PipeDistance)
    {
        m_SavedPosition = m_Bird.Body->Position.x;
        float pipePosition = m_Bird.Body->Position.x + 36.0f;
        int type = Random::Integer(0, 2);

        float lowerPosition = 0.0f;
        float lowerSize = 0.0f;
        float upperPosition = 0.0f;
        float upperSize = 0.0f;
        switch (type)
        {
        case 0:
            lowerPosition = 9.0f;
            lowerSize = 18.0f;
            upperPosition = 33.0f;
            upperSize = 18.0f;
            break;
        case 1:
            lowerPosition = 12.0f;
            lowerSize = 24.0f;
            upperPosition = 36.0f;
            upperSize = 12.0f;
            break;
        case 2:
            lowerPosition = 6.0f;
            lowerSize = 12.0f;
            upperPosition = 30.0f;
            upperSize = 24.0f;
            break;
        }
        Elysium::BodyHandle handle;

        e_PhysicsSystem.createPhysicalBody(&handle, Elysium::BodyType::STATIC, "Lower", 0.0f,
            { pipePosition, lowerPosition }, { 3.0f, lowerSize });
        m_LowerPipes.push_back(handle);
        e_PhysicsSystem.createPhysicalBody(&handle, Elysium::BodyType::STATIC, "Upper", 0.0f,
            { pipePosition, upperPosition }, { 3.0f, upperSize });
        m_UpperPipes.push_back(handle);

        m_BackgroundPosition += 22.5f;
        m_BackgroundPositions.push_back(m_BackgroundPosition);
    }

    m_Camera.setPosition({ m_Bird.Body->Position.x , m_Bird.Body->Position.y, 0.0});

    Elysium::PhysicalBody* lowerPipe = e_PhysicsSystem.getPhysicalBody(m_LowerPipes[m_PipeIndex]);
    Elysium::PhysicalBody* upperPipe = e_PhysicsSystem.getPhysicalBody(m_UpperPipes[m_PipeIndex]);
    if (m_Bird.Body->Position.x > lowerPipe->Position.x && m_Bird.Body->Position.x > upperPipe->Position.x)
    {
        m_PipeIndex++;
    }

    Elysium::Renderer2D::beginScene(m_Camera);
    for (float position : m_BackgroundPositions)
    {
        Elysium::Renderer2D::drawQuad({ position, 21.0f }, { 22.5f, 40.0f }, m_Background);
    }
    Elysium::Renderer2D::drawQuadWithRotation(m_Bird.Body->Position, { 3.0f, 3.0f }, m_Bird.Body->Rotation, m_BirdSprite);

    for (size_t i = 0; i < m_UpperPipes.size(); i++)
    {
        Elysium::PhysicalBody* body = e_PhysicsSystem.getPhysicalBody(m_LowerPipes[i]);
        Elysium::Renderer2D::drawQuad(body->Position, body->getSize(), m_LowerSprite);

        body = e_PhysicsSystem.getPhysicalBody(m_UpperPipes[i]);
        Elysium::Renderer2D::drawQuad(body->Position, body->getSize(), m_UpperSprite);
    }

    Elysium::Renderer2D::drawQuad({ m_Bird.Body->Position.x, m_Ground->Position.y }, m_Ground->getSize(), m_GroundSprite);
    Elysium::Renderer2D::drawQuad({ m_Bird.Body->Position.x, m_Ceiling->Position.y }, m_Ceiling->getSize(), { 0.0f, 0.0f, 1.0f, 1.0f });
    Elysium::Renderer2D::endScene();

    ImGui::Begin("Flappy Bird");
    ImGui::Text("Score: %d", m_PipeIndex);
    if (m_GameOver)
        ImGui::Text("Game Over!");
    ImGui::End();
}

void FlappyBirdScene::onEvent(Elysium::Event& event)
{
    m_Bird.onEvent(event);

    Elysium::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Elysium::WindowResizeEvent>(BIND_EVENT_FUNCTION(FlappyBirdScene::onWindowResizeEvent));
}

bool FlappyBirdScene::onWindowResizeEvent(Elysium::WindowResizeEvent& event)
{
    unsigned int width = event.getWidth();
    unsigned int height = event.getHeight();
    if (width > 0 && height > 0)
        m_Camera.setProjection(-m_Height * (float)(width / height), m_Height * (float)(width / height),
            -m_Height * 0.5f, m_Height * 0.5f);
    return false;
}