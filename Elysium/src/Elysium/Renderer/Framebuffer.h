#pragma once

namespace Elysium
{

    struct FramebufferSpecification
    {
        unsigned int Width, Height;
        unsigned int Samples = 1;

        bool SwapChainTarget = false;
    };

    class Framebuffer
    {
    private:
        unsigned int m_RendererID = 0;
        unsigned int m_ColorAttachment = 0;
        unsigned int m_DepthAttachment = 0;
        FramebufferSpecification m_Specification;

    public:
        Framebuffer(const FramebufferSpecification& specification);
        ~Framebuffer();

        void bind();
        void unbind();

        void resize(unsigned int width, unsigned int height);

        inline unsigned int getColorAttachmentID() const { return m_ColorAttachment; };

        inline const FramebufferSpecification& getSpecification() const { return m_Specification; };

        void Invalidate();
    };
}