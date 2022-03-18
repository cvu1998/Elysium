#include "SandboxScene.h"

SandboxScene::SandboxScene(unsigned int width, unsigned int height) : Elysium::Scene("Sandbox"),
    m_Camera(-m_Height * (float)(width / height), m_Height * (float)(width / height), -m_Height * 0.5f, m_Height * 0.5f),
    m_ParticleSystem(25000, Elysium::UpdateDevice::CPU),
    m_Player({ { -12.5f, 20.0f } })
{
    m_Textures.reserve(12);
    m_Textures.emplace_back("res/texture/soccer_ball.png");
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

    m_Background = m_Textures[3].getTextureData();

    m_Particle.Position = { 0.0f, 0.0f };
    m_Particle.Velocity = { 0.0f, 0.0f };
    m_Particle.VelocityVariation = { 1.0f, 1.0f };
    m_Particle.RotationSpeed = 180.0f;

    m_Particle.ColorBegin = { 0.0f, 0.0f, 1.0f, 1.0f };
    m_Particle.ColorEnd = { 0.0f, 0.0f, 0.0f, 1.0f };
    m_Particle.TextureData = m_Textures[0].getTextureData();

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

    m_Player.m_IdleTexture = m_Textures[1].getTextureData();
    m_Player.m_TextureData = m_Player.m_IdleTexture;

    for (size_t i = 4; i < m_Textures.size(); i++)
        m_Player.RunAnimation.Textures[i - 4] = m_Textures[i].getTextureData();

    m_GroundTexture = m_Textures[2].getTextureData();
    m_GroundTexture.subtextureCoordinates({ 0, 6 }, { 128, 128 });

    m_BoxTexture = m_Textures[2].getTextureData();
    m_BoxTexture.subtextureCoordinates({ 4, 1 }, { 128, 128 });

    m_BallTexture = m_Textures[0].getTextureData();

    e_PhysicsSystem2D.createPhysicalBody(&m_Ground, Elysium::BodyType::STATIC, Elysium::Collider::QUAD, "Ground", 10.0f, { 0.0f, 0.0f }, { 500.0f, 2.0f });
    e_PhysicsSystem2D.createPhysicalBody(&m_MoveableBox, Elysium::BodyType::DYNAMIC, Elysium::Collider::QUAD, "Box", 10.0f, { 4.5f, 25.0f }, { 2.0f, 2.0f });
    e_PhysicsSystem2D.createPhysicalBody(&m_Box, Elysium::BodyType::STATIC, Elysium::Collider::QUAD, "Static-Box-Left", 10.0f, { 2.5f, 2.0f }, { 2.0f, 2.0f });
    e_PhysicsSystem2D.createPhysicalBody(&m_sBox, Elysium::BodyType::STATIC, Elysium::Collider::QUAD, "Static-Box-Right", 10.0f, { 4.5f, 2.0f }, { 2.0f, 2.0f });

    e_PhysicsSystem2D.createPhysicalBody(&m_Ball, Elysium::BodyType::DYNAMIC, Elysium::Collider::CIRCLE, "Ball", 1.0f, { -2.0f, 10.0f }, { 2.0f, 2.0f });
    e_PhysicsSystem2D.createPhysicalBody(&m_Circle, Elysium::BodyType::DYNAMIC, Elysium::Collider::CIRCLE, "Circle", 1.0f, { -5.0f, 10.0f }, { 2.0f, 2.0f });

    Elysium::PhysicalBody2D* ground = e_PhysicsSystem2D.getPhysicalBody(m_Ground);
    ground->setFrictionCoefficient(1.0f);
    //ground->Rotation = glm::radians(10.0f);

    Elysium::PhysicalBody2D* ball = e_PhysicsSystem2D.getPhysicalBody(m_Ball);
    ball->setElasticityCoefficient(1.0f);

    Elysium::PhysicalBody2D* circle = e_PhysicsSystem2D.getPhysicalBody(m_Circle);

    m_Player.Ball = e_PhysicsSystem2D.getPhysicalBody(m_Ball);

    //--- PERCEPTRON ---//
    Elysium::Perceptron perceptron(Elysium::AI::Activation::STEP);
    Elysium::Matrix weights;

    constexpr size_t epochs = 5;

    Elysium::Matrix ANDGateData({ { 0.0f, 0.0f, 0.0f }, 
                                { 1.0f, 0.0f, 0.0f }, 
                                { 0.0f, 1.0f, 0.0f }, 
                                { 1.0f, 1.0f, 1.0f } });

    perceptron.fit(
        Elysium::Matrix::Slice(ANDGateData, 0, 0, 0, 2),
        Elysium::Matrix::Slice(ANDGateData, 0, 0, 2, 3), 
        epochs);

    std::vector<float> results;
    float meanAccuracy = perceptron.score(
        Elysium::Matrix::Slice(ANDGateData, 0, 0, 0, 2),
        Elysium::Matrix::Slice(ANDGateData, 0, 0, 2, 3), 
        results);

    for (float y : results)
        ELY_INFO("AND Gate: {0}", y);
    ELY_INFO("Mean Accuracy for AND: {0}", meanAccuracy);

    Elysium::Matrix ORGateData({ { 0.0f, 0.0f, 0.0f },
                                { 1.0f, 0.0f, 1.0f },
                                { 0.0f, 1.0f, 1.0f },
                                { 1.0f, 1.0f, 1.0f } });

    perceptron.fit(
        Elysium::Matrix::Slice(ORGateData, 0, 0, 0, 2),
        Elysium::Matrix::Slice(ORGateData, 0, 0, 2, 3), 
        epochs);

    results.clear();
    meanAccuracy = perceptron.score(
        Elysium::Matrix::Slice(ORGateData, 0, 0, 0, 2),
        Elysium::Matrix::Slice(ORGateData, 0, 0, 2, 3),
        results);

    for (float y : results)
        ELY_INFO("OR Gate: {0}", y);
    ELY_INFO("Mean Accuracy for OR: {0}", meanAccuracy);

    Elysium::Matrix XORGateData({ { 0.0f, 0.0f, 0.0f },
                                { 0.0f, 1.0f, 1.0f },
                                { 1.0f, 0.0f, 1.0f },
                                { 1.0f, 1.0f, 0.0f } });

    perceptron.fit(
        Elysium::Matrix::Slice(XORGateData, 0, 0, 0, 2),
        Elysium::Matrix::Slice(XORGateData, 0, 0, 2, 3), 
        epochs);

    results.clear();
    meanAccuracy = perceptron.score(
        Elysium::Matrix::Slice(XORGateData, 0, 0, 0, 2),
        Elysium::Matrix::Slice(XORGateData, 0, 0, 2, 3),
        results);

    for (float y : results)
        ELY_INFO("XOR Gate: {0}", y);
    ELY_INFO("Mean Accuracy for XOR: {0}", meanAccuracy);
    //--- PERCEPTRON ---//

    //--- NEURAL NETWORK MODEL ---//
    Elysium::AI::LeakySlope = 0.1f;

    Elysium::Model model(2);
    model.add(new Elysium::Dense(3, Elysium::AI::Activation::LEAKY_RELU, Elysium::AI::Initializer::HE));
    model.add(new Elysium::Dense(1, Elysium::AI::Activation::LEAKY_RELU, Elysium::AI::Initializer::HE));
    model.LossFunction = Elysium::AI::Loss::MEAN_SQUARED;

    Elysium::Matrix::Slice(XORGateData, 0, 0, 0, 2).print();
    Elysium::Matrix::Slice(XORGateData, 0, 0, 2, 3).print();

    model.LearningRate = 0.02f;

    model.Verbose = false;
    model.fit(
        Elysium::Matrix::Slice(XORGateData, 0, 0, 0, 2),
        Elysium::Matrix::Slice(XORGateData, 0, 0, 2, 3),
        10000);

    model.summary();

    Elysium::Matrix result;
    ELY_INFO("Mean Error: {0}", model.score(
        Elysium::Matrix::Slice(XORGateData, 0, 0, 0, 2),
        Elysium::Matrix::Slice(XORGateData, 0, 0, 2, 3),
        result));
    result.print();

    Elysium::Matrix IrisInputs({ 
        {5.1f, 3.5f, 1.4f, 0.2f}, 
        {4.9f, 3.0f, 1.4f, 0.2f},
        {4.7f, 3.2f, 1.3f, 0.2f},
        {4.6f, 3.1f, 1.5f, 0.2f},
        {5.0f, 3.6f, 1.4f, 0.2f},
        {5.4f, 3.9f, 1.7f, 0.4f},
        {4.6f, 3.4f, 1.4f, 0.3f},
        {5.0f, 3.4f, 1.5f, 0.2f},
        {4.4f, 2.9f, 1.4f, 0.2f},
        {4.9f, 3.1f, 1.5f, 0.1f},
        {5.4f, 3.7f, 1.5f, 0.2f},
        {4.8f, 3.4f, 1.6f, 0.2f},
        {4.8f, 3.0f, 1.4f, 0.1f},
        {4.3f, 3.0f, 1.1f, 0.1f},
        {5.8f, 4.0f, 1.2f, 0.2f},
        {5.7f, 4.4f, 1.5f, 0.4f},
        {5.4f, 3.9f, 1.3f, 0.4f},
        {5.1f, 3.5f, 1.4f, 0.3f},
        {5.7f, 3.8f, 1.7f, 0.3f},
        {5.1f, 3.8f, 1.5f, 0.3f},
        {5.4f, 3.4f, 1.7f, 0.2f},
        {5.1f, 3.7f, 1.5f, 0.4f},
        {4.6f, 3.6f, 1.0f, 0.2f},
        {5.1f, 3.3f, 1.7f, 0.5f},
        {4.8f, 3.4f, 1.9f, 0.2f},
        {5.0f, 3.0f, 1.6f, 0.2f},
        {5.0f, 3.4f, 1.6f, 0.4f},
        {5.2f, 3.5f, 1.5f, 0.2f},
        {5.2f, 3.4f, 1.4f, 0.2f},
        {4.7f, 3.2f, 1.6f, 0.2f},
        {4.8f, 3.1f, 1.6f, 0.2f},
        {5.4f, 3.4f, 1.5f, 0.4f},
        {5.2f, 4.1f, 1.5f, 0.1f},
        {5.5f, 4.2f, 1.4f, 0.2f},
        {4.9f, 3.1f, 1.5f, 0.2f},
        {5.0f, 3.2f, 1.2f, 0.2f},
        {5.5f, 3.5f, 1.3f, 0.2f},
        {4.9f, 3.6f, 1.4f, 0.1f},
        {4.4f, 3.0f, 1.3f, 0.2f},
        {5.1f, 3.4f, 1.5f, 0.2f},
        {5.0f, 3.5f, 1.3f, 0.3f},
        {4.5f, 2.3f, 1.3f, 0.3f},
        {4.4f, 3.2f, 1.3f, 0.2f},
        {5.0f, 3.5f, 1.6f, 0.6f},
        {5.1f, 3.8f, 1.9f, 0.4f},
        {4.8f, 3.0f, 1.0f, 0.3f},
        {5.1f, 3.8f, 1.6f, 0.2f},
        {4.6f, 3.2f, 1.4f, 0.2f},
        {5.3f, 3.7f, 1.5f, 0.2f},
        {5.0f, 3.3f, 1.4f, 0.2f},
        {7.0f, 3.2f, 4.7f, 1.4f},
        {6.4f, 3.2f, 4.5f, 1.5f},
        {6.9f, 3.1f, 4.9f, 1.5f},
        {5.5f, 2.3f, 4.0f, 1.3f},
        {6.5f, 2.8f, 4.6f, 1.5f},
        {5.7f, 2.8f, 4.5f, 1.3f},
        {6.3f, 3.3f, 4.7f, 1.6f},
        {4.9f, 2.4f, 3.3f, 1.0f},
        {6.6f, 2.9f, 4.6f, 1.3f},
        {5.2f, 2.7f, 3.9f, 1.4f},
        {5.0f, 2.0f, 3.5f, 1.0f},
        {5.9f, 3.0f, 4.2f, 1.5f},
        {6.0f, 2.2f, 4.0f, 1.0f},
        {6.1f, 2.9f, 4.7f, 1.4f},
        {5.6f, 2.9f, 3.6f, 1.3f},
        {6.7f, 3.1f, 4.4f, 1.4f},
        {5.6f, 3.0f, 4.5f, 1.5f},
        {5.8f, 2.7f, 4.1f, 1.0f},
        {6.2f, 2.2f, 4.5f, 1.5f},
        {5.6f, 2.5f, 3.9f, 1.1f},
        {5.9f, 3.2f, 4.8f, 1.8f},
        {6.1f, 2.8f, 4.0f, 1.3f},
        {6.3f, 2.5f, 4.9f, 1.5f},
        {6.1f, 2.8f, 4.7f, 1.2f},
        {6.4f, 2.9f, 4.3f, 1.3f},
        {6.6f, 3.0f, 4.4f, 1.4f},
        {6.8f, 2.8f, 4.8f, 1.4f},
        {6.7f, 3.0f, 5.0f, 1.7f},
        {6.0f, 2.9f, 4.5f, 1.5f},
        {5.7f, 2.6f, 3.5f, 1.0f},
        {5.5f, 2.4f, 3.8f, 1.1f},
        {5.5f, 2.4f, 3.7f, 1.0f},
        {5.8f, 2.7f, 3.9f, 1.2f},
        {6.0f, 2.7f, 5.1f, 1.6f},
        {5.4f, 3.0f, 4.5f, 1.5f},
        {6.0f, 3.4f, 4.5f, 1.6f},
        {6.7f, 3.1f, 4.7f, 1.5f},
        {6.3f, 2.3f, 4.4f, 1.3f},
        {5.6f, 3.0f, 4.1f, 1.3f},
        {5.5f, 2.5f, 4.0f, 1.3f},
        {5.5f, 2.6f, 4.4f, 1.2f},
        {6.1f, 3.0f, 4.6f, 1.4f},
        {5.8f, 2.6f, 4.0f, 1.2f},
        {5.0f, 2.3f, 3.3f, 1.0f},
        {5.6f, 2.7f, 4.2f, 1.3f},
        {5.7f, 3.0f, 4.2f, 1.2f},
        {5.7f, 2.9f, 4.2f, 1.3f},
        {6.2f, 2.9f, 4.3f, 1.3f},
        {5.1f, 2.5f, 3.0f, 1.1f},
        {5.7f, 2.8f, 4.1f, 1.3f},
        {6.3f, 3.3f, 6.0f, 2.5f},
        {5.8f, 2.7f, 5.1f, 1.9f},
        {7.1f, 3.0f, 5.9f, 2.1f},
        {6.3f, 2.9f, 5.6f, 1.8f},
        {6.5f, 3.0f, 5.8f, 2.2f},
        {7.6f, 3.0f, 6.6f, 2.1f},
        {4.9f, 2.5f, 4.5f, 1.7f},
        {7.3f, 2.9f, 6.3f, 1.8f},
        {6.7f, 2.5f, 5.8f, 1.8f},
        {7.2f, 3.6f, 6.1f, 2.5f},
        {6.5f, 3.2f, 5.1f, 2.0f},
        {6.4f, 2.7f, 5.3f, 1.9f},
        {6.8f, 3.0f, 5.5f, 2.1f},
        {5.7f, 2.5f, 5.0f, 2.0f},
        {5.8f, 2.8f, 5.1f, 2.4f},
        {6.4f, 3.2f, 5.3f, 2.3f},
        {6.5f, 3.0f, 5.5f, 1.8f},
        {7.7f, 3.8f, 6.7f, 2.2f},
        {7.7f, 2.6f, 6.9f, 2.3f},
        {6.0f, 2.2f, 5.0f, 1.5f},
        {6.9f, 3.2f, 5.7f, 2.3f},
        {5.6f, 2.8f, 4.9f, 2.0f},
        {7.7f, 2.8f, 6.7f, 2.0f},
        {6.3f, 2.7f, 4.9f, 1.8f},
        {6.7f, 3.3f, 5.7f, 2.1f},
        {7.2f, 3.2f, 6.0f, 1.8f},
        {6.2f, 2.8f, 4.8f, 1.8f},
        {6.1f, 3.0f, 4.9f, 1.8f},
        {6.4f, 2.8f, 5.6f, 2.1f},
        {7.2f, 3.0f, 5.8f, 1.6f},
        {7.4f, 2.8f, 6.1f, 1.9f},
        {7.9f, 3.8f, 6.4f, 2.0f},
        {6.4f, 2.8f, 5.6f, 2.2f},
        {6.3f, 2.8f, 5.1f, 1.5f},
        {6.1f, 2.6f, 5.6f, 1.4f},
        {7.7f, 3.0f, 6.1f, 2.3f},
        {6.3f, 3.4f, 5.6f, 2.4f},
        {6.4f, 3.1f, 5.5f, 1.8f},
        {6.0f, 3.0f, 4.8f, 1.8f},
        {6.9f, 3.1f, 5.4f, 2.1f},
        {6.7f, 3.1f, 5.6f, 2.4f},
        {6.9f, 3.1f, 5.1f, 2.3f},
        {5.8f, 2.7f, 5.1f, 1.9f},
        {6.8f, 3.2f, 5.9f, 2.3f},
        {6.7f, 3.3f, 5.7f, 2.5f},
        {6.7f, 3.0f, 5.2f, 2.3f},
        {6.3f, 2.5f, 5.0f, 1.9f},
        {6.5f, 3.0f ,5.2f, 2.0f},
        {6.2f, 3.4f, 5.4f, 2.3f},
        {5.9f, 3.0f, 5.1f, 1.8f } });

    std::vector<int> IrisOutputs = { 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 
    };

    Elysium::Matrix OneHotIrisOutputs(IrisOutputs.size(), 3);
    for (size_t i = 0; i < IrisOutputs.size(); ++i)
    {
        switch (IrisOutputs[i])
        {
        case 0:
            OneHotIrisOutputs(i, 0) = 1.0f;
            OneHotIrisOutputs(i, 1) = 0.0f;
            OneHotIrisOutputs(i, 2) = 0.0f;
            break;
        case 1:
            OneHotIrisOutputs(i, 0) = 0.0f;
            OneHotIrisOutputs(i, 1) = 1.0f;
            OneHotIrisOutputs(i, 2) = 0.0f;
            break;
        case 2:
            OneHotIrisOutputs(i, 0) = 0.0f;
            OneHotIrisOutputs(i, 1) = 0.0f;
            OneHotIrisOutputs(i, 2) = 1.0f;
            break;
        }
    }

    Elysium::Model IrisModel(4);
    IrisModel.add(new Elysium::Dense(4, Elysium::AI::Activation::LEAKY_RELU, Elysium::AI::Initializer::HE));
    IrisModel.add(new Elysium::Dense(4, Elysium::AI::Activation::LEAKY_RELU, Elysium::AI::Initializer::HE));
    IrisModel.add(new Elysium::Dense(3, Elysium::AI::Activation::SIGMOID));
    IrisModel.LossFunction = Elysium::AI::Loss::MEAN_SQUARED;

    IrisModel.LearningRate = 0.01f;

    Elysium::Matrix IrisData = Elysium::Matrix::Concatenate(IrisInputs, OneHotIrisOutputs, false);
    IrisData = Elysium::Matrix::Scramble(IrisData);

    /*
    IrisModel.fit(
        Elysium::Matrix::Slice(IrisData, 0, 145, 0, 4),
        Elysium::Matrix::Slice(IrisData, 0, 145, 4, 0),
        50000,
        10);
    IrisModel.save("res/AI/iris-model");
    */

    IrisModel.load("res/AI/iris-model");
    IrisModel.summary();

    Elysium::Matrix IrisResults;
    ELY_INFO("Mean Error: {0}", IrisModel.score(
        Elysium::Matrix::Slice(IrisData, 145, 150, 0, 4),
        Elysium::Matrix::Slice(IrisData, 145, 150, 4, 0),
        IrisResults));

    Elysium::Matrix::Slice(IrisData, 145, 150, 4, 0).print();
    IrisResults.print();

    Elysium::Model QuadraticModel(2);
    QuadraticModel.add(new Elysium::Dense(10, Elysium::AI::Activation::LEAKY_RELU, Elysium::AI::Initializer::HE));
    QuadraticModel.add(new Elysium::Dense(10, Elysium::AI::Activation::LEAKY_RELU, Elysium::AI::Initializer::HE));
    QuadraticModel.add(new Elysium::Dense(1, Elysium::AI::Activation::LINEAR));
    QuadraticModel.LossFunction = Elysium::AI::Loss::MEAN_SQUARED;

    QuadraticModel.LearningRate = 0.000001f;

    QuadraticModel.summary();

    std::vector<std::vector<float>> InputVector;
    std::vector<std::vector<float>> OutputVector;
    for (float x = -10.0f; x <= 10.0f; x += 0.01f)
    {
        InputVector.push_back({ x, x });
        OutputVector.push_back({ x * x });
    }

    Elysium::Matrix InputData(InputVector);
    Elysium::Matrix OutputData(OutputVector);

    /*
    QuadraticModel.fit(InputData, OutputData, 15000, 50);
    QuadraticModel.save("res/AI/quadratic-model");
    */

    QuadraticModel.load("res/AI/quadratic-model");

    Elysium::Matrix ScoreResults;
    ELY_INFO("Mean Error: {0}", QuadraticModel.score(
        InputData,
        OutputData,
        ScoreResults));

    std::vector<std::vector<float>> TestVector;
    TestVector.push_back({-12.0f, -12.0f });
    TestVector.push_back({-11.0f, -11.0f });
    TestVector.push_back({ -7.0f,  -7.0f });
    TestVector.push_back({ -6.0f,  -6.0f });
    TestVector.push_back({ -5.0f,  -5.0f });
    TestVector.push_back({ -4.0f,  -4.0f });
    TestVector.push_back({ -3.0f,  -3.0f });
    TestVector.push_back({ -2.0f,  -2.0f });
    TestVector.push_back({ -1.0f,  -1.0f });
    TestVector.push_back({  0.0f,   0.0f });
    TestVector.push_back({  1.0f,   1.0f });
    TestVector.push_back({  2.0f,   2.0f });
    TestVector.push_back({  3.0f,   3.0f });
    TestVector.push_back({  4.0f,   4.0f });
    TestVector.push_back({  5.0f,   5.0f });
    TestVector.push_back({  6.0f,   6.0f });
    TestVector.push_back({  7.0f,   7.0f });
    TestVector.push_back({ 11.0f,  11.0f });
    TestVector.push_back({ 12.0f,  12.0f });

    Elysium::Matrix TestData(TestVector);
    Elysium::Matrix Predictions;
    QuadraticModel.predict(TestData, Predictions);
    Predictions.print();
    //--- DENSE LAYER ---//
}

SandboxScene::~SandboxScene()
{
    e_PhysicsSystem2D.clear();
}

void SandboxScene::onUpdate(Elysium::Timestep ts)
{
    const Elysium::PhysicalBody2D* player = m_Player.getBody();
    const Elysium::PhysicalBody2D& ground = e_PhysicsSystem2D.readPhysicalBody(m_Ground);
    const Elysium::PhysicalBody2D& box = e_PhysicsSystem2D.readPhysicalBody(m_MoveableBox);
    const Elysium::PhysicalBody2D& sBox1 = e_PhysicsSystem2D.readPhysicalBody(m_Box);
    const Elysium::PhysicalBody2D& sBox2 = e_PhysicsSystem2D.readPhysicalBody(m_sBox);
    const Elysium::PhysicalBody2D& ball = e_PhysicsSystem2D.readPhysicalBody(m_Ball);
    const Elysium::PhysicalBody2D& circle = e_PhysicsSystem2D.readPhysicalBody(m_Circle);

    m_Camera.setPosition({ player->Position.x, player->Position.y + (player->getSize().y * 4.0f), 0.0f });

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
    e_PhysicsSystem2D.onUpdate(ts);

    m_Player.onUpdate(ts);

    Elysium::Renderer::beginScene(m_Camera);
    Elysium::Renderer::drawQuad({ 0.0f, 15.0f }, { 1000.0f, 30.0f }, m_Background, { 15.0f, 1.0f });
    Elysium::Renderer::endScene();

    m_ParticleSystem.onUpdate(ts);
    m_ParticleSystem.onRender(m_Camera);

    Elysium::Renderer::beginScene(m_Camera);
    Elysium::Renderer::drawQuad(player->Position, player->getSize(), m_Player.m_TextureData);
    Elysium::Renderer::drawQuadWithRotation(ground.Position, ground.getSize(), ground.Rotation, m_GroundTexture);
    Elysium::Renderer::drawQuadWithRotation(box.Position, box.getSize(), box.Rotation, m_BoxTexture);
    Elysium::Renderer::drawQuad(sBox1.Position, sBox1.getSize(), m_BoxTexture);
    Elysium::Renderer::drawQuad(sBox2.Position, sBox2.getSize(), m_BoxTexture);
    Elysium::Renderer::drawQuadWithRotation(ball.Position, ball.getSize(), ball.Rotation, m_BallTexture);
    Elysium::Renderer::drawQuadWithRotation(circle.Position, circle.getSize(), circle.Rotation, m_BallTexture);
    Elysium::Renderer::endScene();

    ImGui::Begin("Statistics");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Number of Draw Calls: %d", Elysium::Renderer::getStats().DrawCount);
    ImGui::Text("Number of Quads: %d", Elysium::Renderer::getStats().QuadCount);
    ImGui::End();

    Elysium::Renderer::resetStats();
}

void SandboxScene::onEvent(Elysium::Event& event)
{
    Elysium::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Elysium::KeyPressedEvent>(BIND_EVENT_FUNCTION(SandboxScene::onKeyPressedEvent));
    dispatcher.Dispatch<Elysium::WindowResizeEvent>(BIND_EVENT_FUNCTION(SandboxScene::onWindowResizeEvent));

    m_Player.onEvent(event);
}

bool SandboxScene::onKeyPressedEvent(Elysium::KeyPressedEvent& event)
{
    return false;
}

bool SandboxScene::onWindowResizeEvent(Elysium::WindowResizeEvent& event)
{
    unsigned int width = event.getWidth();
    unsigned int height = event.getHeight();
    if (width > 0 && height > 0)
    m_Camera.setProjection(-m_Height * (float)(width / height), m_Height * (float)(width / height),
        -m_Height * 0.5f, m_Height * 0.5f);
    return false;
}