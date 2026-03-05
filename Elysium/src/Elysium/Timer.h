#pragma once

#include <chrono>
#include <iostream>

namespace Elysium
{
    // To be used in conjunction with ScopedTimer
    class BenchmarkTimer
    {
    private:
        double m_TotalTime = 0.0;
        unsigned int m_Count = 0;

    public:
        BenchmarkTimer() = default;
        ~BenchmarkTimer() = default;

        void addTime(double time) { m_TotalTime += time; }
        void incrementCount() { m_Count++; }

        double getTotalTime() const { return m_TotalTime; }
        double getAverageElaspedTime() const { return m_TotalTime / m_Count; }
    };

    class ScopedTimer
    {
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;

        const char* m_Message = nullptr;
        BenchmarkTimer* m_Timer;

    public:
        ScopedTimer(const char* message);
        ScopedTimer(const char* message, BenchmarkTimer& timer);
        ~ScopedTimer();
    };
}