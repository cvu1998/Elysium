#pragma once

namespace Elysium
{
    class Timestep
    {
    private:
        float m_Time;

    public:
        Timestep(float time = 0.0f) : m_Time(time)
        {
        }

        operator float() const { return m_Time; }

        inline float getSeconds() const { return m_Time; };
        inline float getMilliSeconds() const { return m_Time * 1000.0f; };
    };

}