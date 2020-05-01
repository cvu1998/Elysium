#pragma once

#include "Test.h"

#include <vector>

namespace test {

    class DynamicBatchRendering_Test : public Test 
    {
    private: 
        std::vector<Texture> m_Textures;

        float m_QuadPosition[2] = { -1.5f, -1.5f };

    public:
        DynamicBatchRendering_Test();
        ~DynamicBatchRendering_Test();

        virtual void onUpdate(float deltaTime) override;
        virtual void onRender() override;
        virtual void onImGuiRender() override;
    };
}
