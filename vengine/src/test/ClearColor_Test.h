#pragma once

#include "Test.h"

namespace test {

    class ClearColor_Test : public Test 
    {
    private:
        float m_Color[4];

    public:
        ClearColor_Test();
        ~ClearColor_Test();

        virtual void onUpdate(float deltaTime) override;
        virtual void onRender(const glm::mat4& proj, const glm::mat4& view) override;
        virtual void onImGuiRender() override;

    };
}
