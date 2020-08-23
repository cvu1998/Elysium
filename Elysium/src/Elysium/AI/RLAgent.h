#pragma once

#include <unordered_map>

#include "State.h"

namespace Elysium
{
    class RLAgent
    {
    private:
        struct hash_pair {
            size_t operator()(const State_Action_Pair& pair) const
            {
                std::string string(pair.State.getCode() + std::to_string((unsigned int)pair.Action));
                return std::hash<std::string>{}(string);
            }
        };

    public:
        std::unordered_map<State_Action_Pair, float, hash_pair> Policy;

        float LearningRate;
        float DiscountFactor;

        float DefaultValue = 0.0f;

    public:
        RLAgent(float learningRate, float discountFactor, float defaultValue);

        float getStateActionPairValue(const State_Action_Pair& pair);
        void updateActionValueQL(State_Action_Pair& pair, const State& nextState, std::vector<Action> actions);
        void updateActionValueES(State_Action_Pair& pair, const State& nextState, std::vector<Action> actions);

        void readPolicyFromFile(const char* filename);
        void savePolicyToFile(const char* filename);
    };
}