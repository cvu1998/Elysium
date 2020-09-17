#pragma once

#ifdef APIENTRY
#undef APIENTRY
#endif

#include <memory>

#define NOMINMAX
#include <spdlog/spdlog.h>
#undef NOMINMAX

namespace Elysium
{
    class Log
    {
    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;

    public:
        static void Init();

        inline static std::shared_ptr<spdlog::logger>& getCoreLogger() { return s_CoreLogger; }
        inline static std::shared_ptr<spdlog::logger>& getClientLogger() { return s_ClientLogger; }
    };
}

// Core log macros
#define ELY_CORE_TRACE(...)    ::Elysium::Log::getCoreLogger()->trace(__VA_ARGS__)
#define ELY_CORE_INFO(...)     ::Elysium::Log::getCoreLogger()->info(__VA_ARGS__)
#define ELY_CORE_WARN(...)     ::Elysium::Log::getCoreLogger()->warn(__VA_ARGS__)
#define ELY_CORE_ERROR(...)    ::Elysium::Log::getCoreLogger()->error(__VA_ARGS__)
#define ELY_CORE_CRITICAL(...) ::Elysium::Log::getCoreLogger()->critical(__VA_ARGS__)
#define ELY_LOG_ASSERT(x, y) \
       if (!x)                   \
       {                         \
         ELY_CORE_ERROR(y); \
         __debugbreak();         \
       }

// Client log macros
#define ELY_TRACE(...)         ::Elysium::Log::getClientLogger()->trace(__VA_ARGS__)
#define ELY_INFO(...)          ::Elysium::Log::getClientLogger()->info(__VA_ARGS__)
#define ELY_WARN(...)          ::Elysium::Log::getClientLogger()->warn(__VA_ARGS__)
#define ELY_ERROR(...)         ::Elysium::Log::getClientLogger()->error(__VA_ARGS__)
#define ELY_CRITICAL(...)      ::Elysium::Log::getClientLogger()->critical(__VA_ARGS__)

