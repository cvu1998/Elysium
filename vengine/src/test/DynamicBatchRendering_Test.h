#pragma once

#include "Test.h"

#include <vector>

#include "Texture.h"

namespace test {

    class DynamicBatchRendering_Test : public Test 
    {
    private: 
        VertexArray m_va;
        VertexBuffer m_vb;
        std::unique_ptr<IndexBuffer> m_ib;
        
        Shader m_Shader;
        std::vector<Texture> m_Textures;

        unsigned int m_IndexCount = 0;
        float m_QuadPosition[2] = { -2.0f, -1.0f };

    public:
        DynamicBatchRendering_Test();
        ~DynamicBatchRendering_Test();

        virtual void onUpdate(float deltaTime) override;
        virtual void onRender(const glm::mat4& proj, const glm::mat4& view) override;
        virtual void onImGuiRender() override;

    };
}
