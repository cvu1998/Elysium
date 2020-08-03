#pragma once

#include <Elysium.h>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

namespace test {

    class Test
    {
    protected:
        static Elysium::OrthographicCameraController* s_CameraController;

    public:
        Test()
        {
            if (!s_CameraController)
                s_CameraController = new Elysium::OrthographicCameraController(1280.0f / 720.0f, 3.0f);
        }

        virtual ~Test() {}

        virtual void onUpdate(float deltaTime) {}
        virtual void onRender() {}
        virtual void onImGuiRender() {}
        virtual void onEvent(Elysium::Event& event);

        static void destoryCamera();
    };

    class TestMenu : public Test
    {
    private:
        Test*& m_CurrentTest;
        std::vector<std::pair<const char*, std::function<Test*()>>>m_Tests;

    public:
        TestMenu(Test*& currentTestPointer);

        void onImGuiRender() override;

        template <typename T>
        void registerTest(const char* name)
        {
            ELY_INFO("Registering test: {0}", name);
            m_Tests.push_back(std::make_pair(name, []() { return new T(); }));
        }
    };
}
