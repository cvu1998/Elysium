#pragma once

#include "Test.h"

#include <vector>

#include "Renderer2D.h"

namespace test {

    class DynamicBatchRendering_Test : public Test 
    {
    private: 
        Shader m_Shader;

        std::vector<Texture> m_Textures;

        float m_QuadPosition[2] = { -2.0f, -1.0f };

    public:
        DynamicBatchRendering_Test();
        ~DynamicBatchRendering_Test();

        virtual void onUpdate(float deltaTime) override;
        virtual void onRender(const glm::mat4& proj, const glm::mat4& view) override;
        virtual void onImGuiRender() override;

    };
}
