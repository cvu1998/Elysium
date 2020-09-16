#pragma once

#include <array>

namespace Elysium
{
    template <size_t N>
    struct BinaryFeatureVector
    {
        std::array<float, N> Vector = { 0.0f };

        size_t hash() const
        {
            float number = Vector[0] == 1.0f ? 1.0f : 0.0f;
            for (size_t i = 1; i < N; i++)
            {
                if (Vector[i] == 1.0f)
                {
                    float value = 1.0f;
                    for (unsigned int exponent = 0; exponent < i; exponent++)
                        value *= 2.0f;
                    number += value;
                }
            }
            return std::hash<float>{}(number);
        }


        bool operator==(const BinaryFeatureVector<N>& vector) const
        {
            size_t count = 0;
            for (size_t i = 0; i < N; i++)
            {
                if (Vector[i] != vector.Vector[i])
                    break;
                count++;
            }
            return count == N;
        }
    };
}