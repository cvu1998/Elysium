#pragma once

#include "Test.h"

enum class OutOfBounds{
    IN_BOUNDS = 0,
    XY = 1,
    X_ONLY = 2,
    Y_ONLY = 3,
};

namespace test {

    class ScreenSaver_Test : public Test
    {
    private:
        float m_Height;
        float m_Width;

        VertexArray m_va;
        std::unique_ptr<VertexBuffer> m_vb;
        VertexBufferLayout m_Layout;
        std::unique_ptr<IndexBuffer> m_ib; 
        Shader m_Shader;

        float r;
        bool inc;

        float m_Translation;
        float x;
        float y;
        bool m_SignX;
        bool m_SignY;
        bool m_Inside;
        OutOfBounds m_Case;

    public:
        ScreenSaver_Test();
        ~ScreenSaver_Test();

        virtual void onUpdate(float deltaTime) override;
        virtual void onRender(const glm::mat4& proj, const glm::mat4& view) override;
        virtual void onImGuiRender() override;
    };
}


