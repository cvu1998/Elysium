#pragma once

#include "Test.h"

namespace test {

    class ClearColor : public Test 
    {
    private:
        float m_Color[4];

    public:
        ClearColor();
        ~ClearColor();

        virtual void onUpdate(float deltaTime) override;
        virtual void onRender() override;
        virtual void onImGuiRender() override;

    };
}
