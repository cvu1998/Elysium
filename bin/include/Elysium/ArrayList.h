#pragma once

#include <vector>

namespace Elysium
{
    template <class T, size_t S>
    class ArrayList
    {
    private:
        std::vector<T*> m_ArrayList;
        size_t m_VectorIndex = 0;
        size_t m_ArrayIndex = 0;
        float m_InverseS = 1 / S;

    public:
        ~ArrayList()
        {
            for (auto* index : m_ArrayList)
            {
                delete[] index;
                index = nullptr;
            }
        }

        void add(const T& element)
        {
            if (m_Index == S)
            {
                m_VectorIndex++;
                m_ArrayIndex = 0;
            }

            if (m_Index == 0)
            {
                m_ArrayList.push_back(new T[S]);
            }
            m_ArrayList[m_VectorIndex][m_ArrayIndex] = element;
            m_ArrayIndex++;
        }

        size_t size()
        {
            return (m_VectorIndex * S) + m_ArrayIndex;
        }

        T& operator[](size_t index)
        {
            return m_ArrayList[index * m_InverseS][index % S];
        }
    };
}