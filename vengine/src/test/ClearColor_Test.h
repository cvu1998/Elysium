#pragma once

#include "Test.h"

namespace test {

    class ClearColor : public Test 
    {
    private:
        float m_Color[4];

    private:
        ClearColor();
        ~ClearColor();

    public:
        ClearColor(const ClearColor&) = delete;

        static ClearColor& getInstance()
        {
            static ClearColor s_Instance;
            return s_Instance;
        }

        virtual void onUpdate(float deltaTime) override;
        virtual void onRender() override;
        virtual void onImGuiRender() override;

    };
}
