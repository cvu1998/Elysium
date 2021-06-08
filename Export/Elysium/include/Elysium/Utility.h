#pragma once

#include <vector>

#include "Elysium/Random.h"

#define BIT(x) (1 << x)

#define BIND_EVENT_FUNCTION(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Elysium
{
    namespace Utility
    {
        void CreateRandomVector(std::vector<float>& vector, size_t size, float min, float max);

        void RandomizeVector(std::vector<float>& vector, float min, float max);
    }
}