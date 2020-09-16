#pragma once

#include <memory>
#include <vector>

namespace Elysium
{
    template <class T>
    class ArrayList
    {
    private:
        static constexpr size_t s_ArraySize = 1000;
        static constexpr double s_InverseSize = (1.0 / (double)s_ArraySize);
        std::vector<T*> m_ArrayList;
        size_t m_VectorIndex = 0;
        size_t m_ArrayIndex = 0;

    public:
        ~ArrayList()
        {
            m_VectorIndex = 0;
            m_ArrayIndex = 0;

            for (size_t i = 0; i < m_ArrayList.size(); i++)
                delete[] m_ArrayList[i];

            m_ArrayList.clear();
        }

        void push_back(const T& element)
        {
            if (m_ArrayIndex == s_ArraySize)
            {
                m_VectorIndex++;
                m_ArrayIndex = 0;
            }

            if (m_ArrayIndex == 0)
            {
                m_ArrayList.push_back(new T[s_ArraySize]);
            }
            m_ArrayList[m_VectorIndex][m_ArrayIndex] = element;
            m_ArrayIndex++;
        }

        void push_back(const T&& element)
        {
            if (m_ArrayIndex == s_ArraySize)
            {
                m_VectorIndex++;
                m_ArrayIndex = 0;
            }

            if (m_ArrayIndex == 0)
            {
                m_ArrayList.push_back(new T[s_ArraySize]);
            }
            m_ArrayList[m_VectorIndex][m_ArrayIndex] = element;
            m_ArrayIndex++;
        }

        void clear()
        {
            m_VectorIndex = 0;
            m_ArrayIndex = 0;

            for (size_t i = 0; i < m_ArrayList.size(); i++)
                delete[] m_ArrayList[i];

            m_ArrayList.clear();
        }

        size_t size()
        {
            return (m_VectorIndex * s_ArraySize) + m_ArrayIndex;
        }

        T& operator[](size_t index)
        {
            return m_ArrayList[(size_t)(index * s_InverseSize)][index % s_ArraySize];
        }

        const T& operator[](size_t index) const
        {
            return m_ArrayList[(size_t)(index * s_InverseSize)][index % s_ArraySize];
        }
    };
}