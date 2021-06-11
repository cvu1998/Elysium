#pragma once

namespace Elysium
{
    namespace AI
    {
        enum class Activation
        {
            STEP = 0,
            SIGMOID
        };

        float Step(float x);

        float Sigmoid(float x);
        float SigmoidDerivative(float x);
    }
}