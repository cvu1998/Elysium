#pragma once

#include <Elysium.h>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

namespace test {

    class Test
    {
    public:
        Test() {}
        virtual ~Test() {}

        virtual void onUpdate(float deltaTime) {}
        virtual void onRender(const glm::mat4& proj, const glm::mat4& view) {}
        virtual void onImGuiRender() {}
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
            std::cout << "Registering test " << name << std::endl;
            m_Tests.push_back(std::make_pair(name, []() { return new T(); }));
        }
    };
}
