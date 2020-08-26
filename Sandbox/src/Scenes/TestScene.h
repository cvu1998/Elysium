#pragma once

#include <Elysium.h>

#include "../test/ClearColor_Test.h"
#include "../test/Texture2D_Test.h"
#include "../test/ScreenSaver_Test.h"
#include "../test/BatchRendering_Test.h"
#include "../test/DynamicBatchRendering_Test.h"

class TestScene : public Elysium::Scene
{
private:
    test::Test* m_CurrentTest;
    test::TestMenu* m_TestMenu;

public:
    TestScene();
    ~TestScene();

    void onUpdate(Elysium::Timestep ts) override;
    void onEvent(Elysium::Event& event) override;
};