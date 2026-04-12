#pragma once

#include <atomic>
#include <vector>

#include "Elysium/Random.h"

#define BIT(x) (1 << x)

#define BIND_EVENT_FUNCTION(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Elysium
{
    enum class UpdateDevice
    {
        CPU = 0,
        CPU_THREAD = 1,
        GPU = 2
    };

    namespace Utility
    {
        void CreateRandomVector(std::vector<float>& vector, size_t size, float min, float max);

        void RandomizeVector(std::vector<float>& vector, float min, float max);
    }

    class SpinLock
    {
    private:
        alignas(std::hardware_destructive_interference_size * 2) std::atomic_flag m_Lock{};

    public:
        void lock() noexcept
        {
            while (m_Lock.test_and_set(std::memory_order_acquire)) m_Lock.wait(true, std::memory_order_relaxed);
        }

        bool try_lock() noexcept
        {
            return !m_Lock.test_and_set(std::memory_order_acquire);
        }

        void unlock() noexcept
        {
            m_Lock.clear(std::memory_order_release);
            m_Lock.notify_one();
        }
    };
}