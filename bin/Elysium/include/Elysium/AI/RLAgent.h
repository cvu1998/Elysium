#pragma once

#include "State.h"

namespace Elysium
{
    class RLAgent
    {
    public:
        float LearningRate;
        float DiscountFactor;

        float DefaultValue = 0.0f;

    public:
        RLAgent(float learningRate, float discountFactor, float defaultValue);
    };
}