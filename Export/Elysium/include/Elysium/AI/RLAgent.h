#pragma once

#include "State.h"

namespace Elysium
{
    class RLAgent
    {
    protected:
        float DefaultValue = 0.0f;

    public:
        float LearningRate;
        float DiscountFactor;

    public:
        RLAgent(float learningRate, float discountFactor, float defaultValue);
    };
}