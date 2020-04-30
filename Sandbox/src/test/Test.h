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
        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;

    public:
        Test()
        {
            m_ProjectionMatrix = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, -1.0f, 1.0f);
            m_ViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        }

        virtual ~Test() {}

        virtual void onUpdate(float deltaTime) {}
        virtual void onRender() {}
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
            std::cout << "Registering test " << name << "\n";
            m_Tests.push_back(std::make_pair(name, []() { return new T(); }));
        }
    };
}
