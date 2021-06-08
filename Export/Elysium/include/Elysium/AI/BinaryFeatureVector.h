#pragma once

#include <array>

namespace Elysium
{
    template <size_t N>
    struct BinaryFeatureVector
    {
        std::array<bool, N> Array = { 0.0f };

        bool operator==(const BinaryFeatureVector<N>& vector) const
        {
            size_t count = 0;
            for (size_t i = 0; i < N; i++)
            {
                if (Array[i] != vector.Array[i])
                    break;
                count++;
            }
            return count == N;
        }

        bool& operator[](size_t index)
        {
            return Array[index];
        }

        bool operator[](size_t index) const
        {
            return Array[index];
        }
    };
}