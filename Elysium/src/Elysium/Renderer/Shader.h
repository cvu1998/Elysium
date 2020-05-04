#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>
#include <string>

#include "Elysium/Utility.h"

struct ShaderProgramSource
{
    std::string vertexSource;
    std::string fragmentSource;
};

class Shader
{
private:
    const char* m_FilePath;
    unsigned int m_RendererID;
    mutable std::unordered_map<std::string, int> m_UniformLocationCache;

public:
    Shader(const char* filepath);
    ~Shader();

    void bind() const;
    void unBind() const;

    void setUniform1i(const char* name, int value);

    template <size_t T>
    void setUniform1iv(const char* name, int value, int(&sampler)[T])
    {
        GL_ASSERT(glUniform1iv(getUniformLocation(name), value, sampler));
    }

    void setUniform4f(const char* name, float v0, float v1, float v2, float v3);
    void setUniformMat4f(const char* name, const glm::mat4& matrix);

private:
    int getUniformLocation(const char* name) const;
    ShaderProgramSource parseShader(const char* filepath);
    unsigned int compileShader(unsigned int type, const std::string& source);
    unsigned int createShader(const std::string& vertexShader, const std::string fragmentShader);
};

