#pragma once

#include "Test.h"

namespace test {

    class ClearColor : public Test 
    {
    public:
        static ClearColor* s_Instance;

    private:
        float m_Color[4];

    private:
        ClearColor();
        ~ClearColor();

    public:
        static ClearColor* getInstance()
        {
            if (!s_Instance)
                s_Instance = new ClearColor();
            return s_Instance;
        }

        virtual void onUpdate(float deltaTime) override;
        virtual void onRender() override;
        virtual void onImGuiRender() override;

    };
}