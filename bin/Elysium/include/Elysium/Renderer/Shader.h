#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>
#include <string>

#include "Elysium/Utility.h"

namespace Elysium
{
    enum class ShaderType
    {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1,
        COMPUTE = 2,
        VERTEX_AND_FRAGMENT = 3
    };

    class Shader
    {
    private:
        struct ShaderProgramSource
        {
            std::string VertexSource;
            std::string FragmentSource;
            std::string ComputeSource;
        };

        const char* m_FilePath;
        unsigned int m_RendererID;
        mutable std::unordered_map<std::string, int> m_UniformLocationCache;

    public:
        Shader(const char* filepath, ShaderType type = ShaderType::VERTEX_AND_FRAGMENT);
        ~Shader();

        void bind() const;
        void unBind() const;

        void setUniform1i(const char* name, int value);

        void setUniform1iv(const char* name, int value, int* sampler);
        void setUniform4f(const char* name, float v0, float v1, float v2, float v3);
        void setUniformMat4f(const char* name, const glm::mat4& matrix);

    private:
        int getUniformLocation(const char* name) const;
        ShaderProgramSource parseShader(const char* filepath);
        unsigned int compileShader(unsigned int type, const std::string& source);
        unsigned int createShader(const ShaderProgramSource& source, ShaderType type);
    };
}