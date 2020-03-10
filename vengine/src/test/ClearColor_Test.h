#pragma once

#include "Test.h"

namespace test {

    class ClearColor : public Test 
    {
    public:
        ClearColor();
        ~ClearColor();

    private:
        virtual void onUpdate(float deltaTime) override;
        virtual void onRender() override;
        virtual void onImGuiRender() override;

    private:
        float m_Color[4];

    };
}