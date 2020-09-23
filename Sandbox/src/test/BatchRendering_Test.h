#pragma once

#include "Test.h"

#include <vector>

namespace test {

    class BatchRendering_Test : public Test 
    {
    private:
        VertexArray m_va;
        std::unique_ptr<VertexBuffer> m_vb;
        std::unique_ptr<IndexBuffer> m_ib;
        Elysium::Shader m_Shader;
        std::vector<Texture> m_Textures;

        glm::vec3 m_Translation;

    public:
        BatchRendering_Test();
        ~BatchRendering_Test();

        virtual void onUpdate(float deltaTime) override;
        virtual void onRender() override;
        virtual void onImGuiRender() override;
    };
}
