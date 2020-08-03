#include "Timer.h"

#include "Elysium/Log.h"

namespace Elysium
{
    Timer::Timer(const char* message) : m_Message(message)
    {
        m_StartTime = std::chrono::high_resolution_clock::now();
    }

    Timer::~Timer()
    {
        auto endTime = std::chrono::high_resolution_clock::now();

        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTime).time_since_epoch().count();
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTime).time_since_epoch().count();

        auto duration = end - start;
        double ms = duration * 0.001;

        ELY_CORE_TRACE("{0}: {1} us, {2} ms ", m_Message, duration, ms);
    }
}