#include "RLAgent.h"

#include <algorithm>
#include <fstream>

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

    float RLAgent::getStateActionPairValue(const State_Action_Pair& pair)
    {
        auto stateActionPair = Policy.find(pair);
        if (stateActionPair == Policy.end())
            Policy[pair] = { DefaultValue };
        return Policy[pair];
    }

    void RLAgent::updateActionValueQL(State_Action_Pair& pair, const State& nextState, std::vector<Action> actions)
    {
        auto stateActionPair = Policy.find(pair);
        if (stateActionPair == Policy.end())
            Policy[pair] = { DefaultValue };

        float maxNextActionValue = -std::numeric_limits<float>::max();
        if (!nextState.isTerminal())
        {
            for (Action action : actions)
            {
                State_Action_Pair nextPair = { nextState, action };
                auto iterator = Policy.find(nextPair);
                if (iterator != Policy.end())
                {
                    maxNextActionValue = std::max(maxNextActionValue, iterator->second);
                }
            }
            maxNextActionValue = maxNextActionValue == -std::numeric_limits<float>::max() ? DefaultValue : maxNextActionValue;
        }
        maxNextActionValue = nextState.isTerminal() ? 0.0f : maxNextActionValue;
        Policy[pair] += LearningRate * (nextState.getReward() + (DiscountFactor * maxNextActionValue) - Policy[pair]);
    }

    void RLAgent::updateActionValueES(State_Action_Pair& pair, const State& nextState, std::vector<Action> actions)
    {
        auto stateActionPair = Policy.find(pair);
        if (stateActionPair == Policy.end())
            Policy[pair] = { DefaultValue };

        float expectedValue = 0.0f;
        float probability = 1.0f / (actions.size() - 1.0f);
        std::vector<float> summationValues;
        if (!nextState.isTerminal())
        {
            for (Action action : actions)
            {
                State_Action_Pair nextPair = { nextState, action };
                auto iterator = Policy.find(nextPair);
                if (iterator != Policy.end())
                {
                    summationValues.push_back(iterator->second);
                }
                else
                {
                    summationValues.push_back(DefaultValue);
                }
            }
            float maxValue = -std::numeric_limits<float>::max();
            for (float value : summationValues)
            {
                maxValue = std::max(maxValue, value);
            }

            for (float value : summationValues)
            {
                if (value == maxValue)
                    expectedValue += value * (1.0f - probability);
                else
                    expectedValue += value * probability;
            }
        }
        expectedValue = nextState.isTerminal() ? 0.0f : expectedValue;
        Policy[pair] += LearningRate * (nextState.getReward() + (DiscountFactor * expectedValue) - Policy[pair]);
    }

    void RLAgent::readPolicyFromFile(const char* filename)
    {
        std::ifstream policyFile(filename);
        if (policyFile.is_open())
        {
            std::string line;
            uint8_t counter = 0;

            State_Action_Pair pair;
            while (getline(policyFile, line))
            {
                switch (counter)
                {
                case 0:
                    pair.State = std::move(State(line));
                    counter++;
                    break;
                case 1:
                    pair.Action = (Action)std::stoul(line);
                    counter++;
                    break;
                case 2:
                    Policy[pair] = std::stof(line);
                    counter = 0;
                    break;
                }
            }
            policyFile.close();
        }
    }

    void RLAgent::savePolicyToFile(const char* filename)
    {
        if (!Policy.empty())
        {
            std::ofstream policyFile(filename);
            std::string string;
            for (auto iterator : Policy)
            {
                iterator.first.toString(string);
                policyFile << string;
                policyFile << iterator.second << std::endl;
            }
            policyFile.close();
        }
    }
}