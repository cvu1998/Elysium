#include "AIUtility.h"

#include <glm/glm.hpp>

namespace Elysium
{
    namespace AI
    {
        float step(float x)
        {
            return x >= 1.0f ? 1.0f : 0.0f;
        }

        float sigmoid(float x)
        {
            return  1.0f / (1.0f + glm::exp(-x));
        }
    }
}