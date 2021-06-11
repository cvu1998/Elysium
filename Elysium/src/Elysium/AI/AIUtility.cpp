#include "AIUtility.h"

#include <glm/glm.hpp>

namespace Elysium
{
    namespace AI
    {
        float Step(float x)
        {
            return x >= 1.0f ? 1.0f : 0.0f;
        }

        float Sigmoid(float x)
        {
            return  1.0f / (1.0f + glm::exp(-x));
        }
        float SigmoidDerivative(float x)
        {
            return x * (1.0f - x);
        }
    }
}