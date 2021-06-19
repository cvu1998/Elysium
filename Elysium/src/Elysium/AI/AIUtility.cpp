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

        float SigmoidDerivative(float sigmoid)
        {
            return sigmoid * (1.0f - sigmoid);
        }

        float Linear(float x)
        {
            return x;
        }

        float LinearDerivative(float x)
        {
            return 1.0f;
        }

        float Forecast(float correct, float prediction)
        {
            return correct - prediction;
        }

        float Mean(float value, size_t n)
        {
            return value / (float)n;
        }

        float MeanSquare(float correct, float prediction)
        {
            float forecast = correct - prediction;
            return forecast * forecast;
        }

        float RootMeanSquare(float value, size_t n)
        {
            return glm::sqrt(value / (2.0f * (float)n));
        }
    }
}