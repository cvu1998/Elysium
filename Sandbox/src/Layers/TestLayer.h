#pragma once

#include "SandboxLayer.h"

#include "../test/ClearColor_Test.h"
#include "../test/Texture2D_Test.h"
#include "../test/ScreenSaver_Test.h"
#include "../test/BatchRendering_Test.h"
#include "../test/DynamicBatchRendering_Test.h"

class TestLayer : public Elysium::Layer
{
private:
    test::Test* m_CurrentTest;
    test::TestMenu* m_TestMenu;

    bool* m_RunSandbox;

public:
    TestLayer(bool* runSandbox);
    ~TestLayer();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;
};
