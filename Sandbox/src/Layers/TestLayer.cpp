#include "Testlayer.h"

TestLayer::TestLayer(bool* runSandbox) : Layer("Tests"), m_RunSandbox(runSandbox)
{
    m_TestMenu = new test::TestMenu(m_CurrentTest);
    m_CurrentTest = m_TestMenu;

    m_TestMenu->registerTest<test::ClearColor_Test>("Clear Color");
    m_TestMenu->registerTest<test::Texture2D_Test>("2D Texture");
    m_TestMenu->registerTest<test::ScreenSaver_Test>("Screen Saver");
    m_TestMenu->registerTest<test::BatchRendering_Test>("Batch Rendering");
    m_TestMenu->registerTest<test::DynamicBatchRendering_Test>("Dynamic Batch Rendering");
}

TestLayer::~TestLayer()
{
    delete m_CurrentTest;
    if (m_CurrentTest != m_TestMenu)
        delete m_TestMenu;
}

void TestLayer::onUpdate(Elysium::Timestep ts)
{
    if (m_CurrentTest)
    {
        if (m_CurrentTest == m_TestMenu)
            *m_RunSandbox = true;

        m_CurrentTest->onUpdate(ts);
        m_CurrentTest->onRender();
        ImGui::Begin("Test");
        if (m_CurrentTest != m_TestMenu)
        {
            *m_RunSandbox = false;
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