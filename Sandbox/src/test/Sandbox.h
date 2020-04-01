#pragma once

#include "Test.h"

#include <vector>

#include "Renderer2D.h"

namespace test {

    class Sandbox : public Test 
    {
    private:
        std::vector<Texture> m_Textures;

        float m_QuadPosition[2] = { -1.5f, -1.5f };

    public:
        Sandbox();
        ~Sandbox();

        virtual void onUpdate(float deltaTime) override;
        virtual void onRender(const glm::mat4& proj, const glm::mat4& view) override;
        virtual void onImGuiRender() override;
    };
}
