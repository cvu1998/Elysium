#pragma once

#include "Test.h"

namespace test {

    class Texture2D_Test : public Test 
    {
    private:
        VertexArray m_va;
        std::unique_ptr<VertexBuffer> m_vb;
        VertexBufferLayout m_Layout;
        std::unique_ptr<IndexBuffer> m_ib;
        Shader m_Shader;
        Texture m_Texture;

        glm::vec3 m_TranslationA;
        glm::vec3 m_TranslationB;

    public:
        Texture2D_Test();
        ~Texture2D_Test();

        virtual void onUpdate(float deltaTime) override;
        virtual void onRender() override;
        virtual void onImGuiRender() override;

    };
}
