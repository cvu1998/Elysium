#include "RLAgent.h"

#include "Elysium/Math.h"

namespace Elysium
{
    RLAgent::RLAgent(float learningRate, float discountFactor, float defaultValue) :
        LearningRate(learningRate),
        DiscountFactor(discountFactor),
        DefaultValue(defaultValue)
    {
        LearningRate = (LearningRate < 0.0f || LearningRate > 1.0f) ? 1.0f : LearningRate;
        DiscountFactor = (DiscountFactor < 0.0f || DiscountFactor > 1.0f) ? 0.0f : DiscountFactor;
    }
}