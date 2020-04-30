#include <iostream>

#include <glad/glad.h>

#define LOG_ASSERT(x, y) \
       if (!x)                   \
       {                         \
         std::cout << y << '\n'; \
         __debugbreak();         \
       }

#ifdef _DEBUG
 #define ASSERT(x) if (!x) __debugbreak();
 #define GL_ASSERT(x) glClearError(); \
         x; \
         ASSERT( glLogCall(#x, __FILE__, __LINE__) )
#else
 #define ASSERT(x) x;
 #define GL_ASSERT(x) x;
#endif

#define BIT(x) (1 << x)

#define BIND_EVENT_FUNCTION(fn) std::bind(&fn, this, std::placeholders::_1)

void glClearError();
bool glLogCall(const char* function, const char* file, int line);

