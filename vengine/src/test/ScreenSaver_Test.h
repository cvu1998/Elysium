#pragma once

#include "Renderer.h"

namespace test {

    class ScreenSaver
    {
    private:
        VertexArray m_va;
        VertexBufferLayout m_Layout;
        IndexBuffer* m_ib; 
        Shader* m_Shader;

        float r;
        bool inc;

        float x;
        float y;
        bool m_SignX;
        bool m_SignY;

    private:
        ScreenSaver();
        ~ScreenSaver();

    public:
        ScreenSaver(const ScreenSaver&) = delete;

        static ScreenSaver* s_Instance;
        static ScreenSaver* getInstance() {
            if (!s_Instance)
                s_Instance = new ScreenSaver();
            return s_Instance;
        }
        void destroyScreenSaver();

        void setScreenSaver(const VertexBuffer& vb);
        void updateScreenSaver(Renderer& renderer, const glm::mat4& proj, const glm::mat4& view);
        void unBind() const;
    };
}


