#pragma once

#include <chrono>
#include <iostream>

namespace Elysium
{
    class Timer
    {
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;

        const char* m_Message;

    public:
        Timer(const char* message);
        ~Timer();
    };
}