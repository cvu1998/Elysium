#include "Utility.h"

#include <iostream>

void glClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool glLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (0x" << std::hex << error << "): " << function << " in file:\n"
            << " " << file << ": " << std::dec << line << std::endl;
        return false;
    }
    return true;
}