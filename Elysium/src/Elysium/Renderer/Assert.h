#ifdef _DEBUG
#define ASSERT(x) if (!x) __debugbreak();
#define GL_ASSERT(x) glClearError(); \
         x; \
         ASSERT( glLogCall(#x, __FILE__, __LINE__) )
#else
#define ASSERT(x) x;
#define GL_ASSERT(x) x;
#endif

void glClearError();
bool glLogCall(const char* function, const char* file, int line);