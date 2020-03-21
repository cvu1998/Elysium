#pragma once

#include "Test.h"

#include <vector>

#include "Texture.h"

namespace test {

    class DynamicBatchRendering_Test : public Test 
    {
    private:
        //unsigned int m_VertexRendererID;
        //unsigned int m_IndexRendererID;
        
        VertexArray m_va;
        VertexBuffer m_vb;
        VertexBufferLayout m_Layout;
        IndexBuffer m_ib;
        
        Shader m_Shader;
        std::vector<Texture> m_Textures;

        glm::vec3 m_Translation;
        float m_QuadPosition[2] = { -1.5f, -1.0f };

    public:
        DynamicBatchRendering_Test();
        ~DynamicBatchRendering_Test();

        virtual void onUpdate(float deltaTime) override;
        virtual void onRender(const glm::mat4& proj, const glm::mat4& view) override;
        virtual void onImGuiRender() override;

    };
}
