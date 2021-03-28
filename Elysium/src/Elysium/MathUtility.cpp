#pragma once

#include <cmath>

#include "Elysium/Math.h"

namespace Elysium
{
    namespace Math
    {
        float cross(const Vector2& a, const Vector2& b)
        {
            return a.x * b.y - a.y * b.x;
        }

        bool isInf(const Vector2& vector)
        {
            return (std::isinf(vector.x) || std::isinf(vector.y));
        }

        bool isInf(const Vector3& vector)
        {
            return (std::isinf(vector.x) || std::isinf(vector.y) || std::isinf(vector.z));
        }

        bool isNaN(const Vector2& vector)
        {
            return (std::isnan(vector.x) || std::isnan(vector.y));
        }

        bool isNaN(const Vector3& vector)
        {
            return (std::isnan(vector.x) || std::isnan(vector.y) || std::isnan(vector.x));
        }
    }
}