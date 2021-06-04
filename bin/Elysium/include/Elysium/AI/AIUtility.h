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

        float step(float x);
        float sigmoid(float x);
    }
}