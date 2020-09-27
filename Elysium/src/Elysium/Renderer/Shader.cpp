#include "Shader.h"

#include <fstream>
#include <sstream>

#include "Elysium/Log.h"

#include <glad/glad.h>

namespace Elysium
{
    Shader::Shader(const char* filepath, ShaderType type) : 
        m_FilePath(filepath),
        m_RendererID(0)
    {
        ShaderProgramSource source = parseShader(filepath);
        m_RendererID = createShader(source, type);
    }

    Shader::~Shader()
    {
        GL_ASSERT(glDeleteProgram(m_RendererID));
    }

    void Shader::bind() const
    {
        GL_ASSERT(glUseProgram(m_RendererID));
    }

    void Shader::unBind() const
    {
        GL_ASSERT(glUseProgram(0));
    }

    void Shader::setUniform1i(const char* name, int value)
    {
        GL_ASSERT(glUniform1i(getUniformLocation(name), value));
    }

    void Shader::setUniform1iv(const char* name, int value, int* sampler)
    {
        GL_ASSERT(glUniform1iv(getUniformLocation(name), value, sampler));
    }

    void Shader::setUniform4f(const char* name, float v0, float v1, float v2, float v3)
    {
        GL_ASSERT(glUniform4f(getUniformLocation(name), v0, v1, v2, v3));
    }

    void Shader::setUniformMat4f(const char* name, const glm::mat4& matrix)
    {
        GL_ASSERT(glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
    }

    int Shader::getUniformLocation(const char* name) const
    {
        if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
            return m_UniformLocationCache[name];

        GL_ASSERT(int location = glGetUniformLocation(m_RendererID, name));
        if (location == -1)
            ELY_CORE_WARN("Warning: Uniform {0} doesn't exist!", name);

        m_UniformLocationCache[name] = location;
        return location;
    }

    Shader::ShaderProgramSource Shader::parseShader(const char* filepath)
    {
        std::ifstream stream(filepath);
        std::string line;
        std::stringstream ss[3];
        ShaderType type = ShaderType::NONE;
        while (getline(stream, line))
        {
            if (line.find("#shader") != std::string::npos)
            {
                if (line.find("vertex") != std::string::npos)
                {
                    type = ShaderType::VERTEX;
                }
                else if (line.find("fragment") != std::string::npos)
                {
                    type = ShaderType::FRAGMENT;
                }
                else if (line.find("compute") != std::string::npos)
                {
                    type = ShaderType::COMPUTE;
                }
            }
            else
            {
                ss[(size_t)type] << line << '\n';
            }
        }
        stream.close();
        return { ss[0].str(), ss[1].str(), ss[2].str() };
    }

    unsigned int Shader::compileShader(unsigned int type, const std::string& source)
    {
        unsigned int id = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        int result;
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
        if (!result)
        {
            int length;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            char* message = (char*)alloca(length * sizeof(char));
            glGetShaderInfoLog(id, length, &length, message);
            const char* shaderType;
            switch (type)
            {
            case GL_VERTEX_SHADER:
                shaderType = "vertex";
                break;
            case GL_FRAGMENT_SHADER:
                shaderType = "fragment";
                break;
            case GL_COMPUTE_SHADER:
                shaderType = "compute";
                break;
            }
            ELY_CORE_ERROR("Failed to compile {0} shader!", shaderType);
            ELY_CORE_ERROR(message);
            glDeleteShader(id);

            return 0;
        }
        return id;
    }

    unsigned int Shader::createShader(const ShaderProgramSource& source, ShaderType type)
    {
        unsigned int program = glCreateProgram();
        switch (type)
        {
        case ShaderType::VERTEX_AND_FRAGMENT:
            {
                unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, source.VertexSource);
                unsigned fragmentShader = compileShader(GL_FRAGMENT_SHADER, source.FragmentSource);

                glAttachShader(program, vertexShader);
                glAttachShader(program, fragmentShader);
                glLinkProgram(program);
                glValidateProgram(program);

                glDeleteShader(vertexShader);
                glDeleteShader(fragmentShader);
            }
            break;
        case ShaderType::COMPUTE:
            {
                unsigned computeShader = compileShader(GL_COMPUTE_SHADER, source.ComputeSource);

                glAttachShader(program, computeShader);
                glLinkProgram(program);
                glValidateProgram(program);

                glDeleteShader(computeShader);
            }
            break;
        }

        return program;
    }
}