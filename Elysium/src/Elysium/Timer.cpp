#include "Timer.h"

#include "Elysium/Log.h"

namespace Elysium
{
    ScopedTimer::ScopedTimer(const char* message) :
        m_Message(message),
        m_Timer(nullptr)
    {
        m_StartTime = std::chrono::high_resolution_clock::now();
    }

    ScopedTimer::ScopedTimer(const char* message, BenchmarkTimer& timer) :
        m_Message(message),
        m_Timer(&timer)
    {
        m_StartTime = std::chrono::high_resolution_clock::now();
    }

    ScopedTimer::~ScopedTimer()
    {
        auto endTime = std::chrono::high_resolution_clock::now();

        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTime).time_since_epoch().count();
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTime).time_since_epoch().count();

        auto duration = end - start;
        double ms = duration * 0.001;

        if (m_Timer)
        {
            m_Timer->incrementCount();
            m_Timer->addTime(ms);
        }

        ELY_CORE_TRACE("{0}: {1} us, {2} ms ", m_Message, duration, ms);
    }
}