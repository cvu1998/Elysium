#include "TestScene.h"

TestScene::TestScene() : Scene("Tests")
{
    m_TestMenu = new test::TestMenu(m_CurrentTest);
    m_CurrentTest = m_TestMenu;

    m_TestMenu->registerTest<test::ClearColor_Test>("Clear Color");
    m_TestMenu->registerTest<test::Texture2D_Test>("2D Texture");
    m_TestMenu->registerTest<test::ScreenSaver_Test>("Screen Saver");
    m_TestMenu->registerTest<test::BatchRendering_Test>("Batch Rendering");
    m_TestMenu->registerTest<test::DynamicBatchRendering_Test>("Dynamic Batch Rendering");
}

TestScene::~TestScene()
{
    delete m_CurrentTest;
    test::Test::destoryCamera();
    if (m_CurrentTest != m_TestMenu)
        delete m_TestMenu;
}

void TestScene::onUpdate(Elysium::Timestep ts)
{
    if (m_CurrentTest)
    {
        m_CurrentTest->onUpdate(ts);
        m_CurrentTest->onRender();
        ImGui::Begin("Test");
        if (m_CurrentTest != m_TestMenu)
        {
            if (ImGui::Button("<--"))
            {
                delete m_CurrentTest;
                m_CurrentTest = m_TestMenu;
            }
        }
        m_CurrentTest->onImGuiRender();
        ImGui::End();
    }
}

void TestScene::onEvent(Elysium::Event& event)
{
    m_CurrentTest->onEvent(event);
}