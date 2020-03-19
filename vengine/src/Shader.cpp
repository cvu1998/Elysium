#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>

enum class ShaderType
{
    NONE = -1,
    VERTEX = 0,
    FRAGMENT = 1
};

Shader::Shader(const char* filepath) : m_FilePath(filepath), m_RendererID(0)
{
    ShaderProgramSource source = parseShader(filepath);
    m_RendererID = createShader(source.vertexSource, source.fragmentSource);
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

void Shader::setUniform4f(const char* name, float v0, float v1, float v2, float v3)
{
    GL_ASSERT(glUniform4f(getUniformLocation(name), v0, v1, v2 ,v3));
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
        std::cout << "Warning: Uniform " << name << " doesn't exist!" << std::endl;

    m_UniformLocationCache[name] = location;
    return location;
}

ShaderProgramSource Shader::parseShader(const char* filepath)
{
    std::ifstream stream(filepath);
    std::string line;
    std::stringstream ss[2];
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
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(), ss[1].str() };
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
        std::cout << "Failed to compile " <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);

        return 0;
    }
    return id;
}

unsigned int Shader::createShader(const std::string& vertexShader, const std::string fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vS = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned fS = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vS);
    glAttachShader(program, fS);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vS);
    glDeleteShader(fS);

    return program;
}