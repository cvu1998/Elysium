#pragma once

#include <cmath>

#include "Elysium/Math.h"

namespace Elysium
{
    namespace Math
    {
        float cross(const Vector2& a, const Vector2& b);

        bool isInf(const Vector2& vector);

        bool isInf(const Vector3& vector);

        bool isNaN(const Vector2& vector);

        bool isNaN(const Vector3& vector);

        template <typename T>
        T normalize(const T& vec)
        {
            T vector = glm::normalize(vec);
            return isNaN(vector) || isInf(normalize(vector)) ? T(0.0f) : vector;
        }

        template <typename T>
        int sgn(T val)
        {
            return (T(0) < val) - (val < T(0));
        }
    }
}