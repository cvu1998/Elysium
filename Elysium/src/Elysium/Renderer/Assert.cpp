#include <glad/glad.h>

#include "Elysium/Log.h"

void glClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool glLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        ELY_CORE_ERROR("[OpenGL Error] (0x{0:x}): {1} in file:\n{2}: {3}", error, function, file, line);
        return false;
    }
    return true;
}