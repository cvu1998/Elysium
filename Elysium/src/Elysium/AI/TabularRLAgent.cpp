#include "TabularRLAgent.h"

#include <algorithm>
#include <fstream>

namespace Elysium
{
    TabularRLAgent::TabularRLAgent(float learningRate, float discountFactor, float defaultValue) :
        RLAgent(learningRate, discountFactor, defaultValue)
    {
    }

    float TabularRLAgent::getStateValue(const State& state)
    {
        if (StateValueFunction.find(state) == StateValueFunction.end())
            StateValueFunction[state] = { DefaultValue };
        return StateValueFunction[state];
    }

    void TabularRLAgent::updateStateValue(const State& currentState, const State& nextState)
    {
        if (StateValueFunction.find(currentState) == StateValueFunction.end())
            StateValueFunction[currentState] = { DefaultValue };

        float nextStateValue = DefaultValue;
        if (!nextState.isTerminal())
        {
            auto iterator = StateValueFunction.find(nextState);
            if (iterator != StateValueFunction.end())
            {
                nextStateValue = iterator->second;
            }
        }

        nextStateValue = nextState.isTerminal() ? 0.0f : nextStateValue;
        StateValueFunction[currentState] += LearningRate * (nextState.getReward() + (DiscountFactor * nextStateValue) - StateValueFunction[currentState]);
    }

    void TabularRLAgent::readFunctionFromFile(const char* filename)
    {
        std::ifstream functionFile(filename);
        if (functionFile.is_open())
        {
            std::string line;
            uint8_t counter = 0;

            State state;
            while (getline(functionFile, line))
            {
                switch (counter)
                {
                case 0:
                    state = std::move(State(line));
                    counter++;
                    break;
                case 1:
                    StateValueFunction[state] = std::stof(line);
                    counter = 0;
                    break;
                }
            }
            functionFile.close();
        }
    }

    void TabularRLAgent::saveFunctionToFile(const char* filename)
    {
        if (!StateValueFunction.empty())
        {
            std::ofstream functionFile(filename);
            for (auto iterator : StateValueFunction)
            {
                functionFile << iterator.first.getCode() << std::endl;
                functionFile << iterator.second << std::endl;
            }
            functionFile.close();
        }
    }

    // ---------- TABULAR CONTROL ----------//

    TabularControlRLAgent::TabularControlRLAgent(float learningRate, float discountFactor, float defaultValue) :
        RLAgent(learningRate, discountFactor, defaultValue)
    {
    }

    float TabularControlRLAgent::getStateActionPairValue(const State_Action_Pair& pair)
    {
        if (ActionValueFunction.find(pair) == ActionValueFunction.end())
            ActionValueFunction[pair] = { DefaultValue };
        return ActionValueFunction[pair];
    }

    void TabularControlRLAgent::updateActionValueQL(const State_Action_Pair& pair, const State& nextState, std::vector<Action> actions)
    {
        if (ActionValueFunction.find(pair) == ActionValueFunction.end())
            ActionValueFunction[pair] = { DefaultValue };

        float maxNextActionValue = -std::numeric_limits<float>::max();
        if (!nextState.isTerminal())
        {
            for (Action action : actions)
            {
                State_Action_Pair nextPair = { nextState, action };
                auto iterator = ActionValueFunction.find(nextPair);
                if (iterator != ActionValueFunction.end())
                {
                    maxNextActionValue = std::max(maxNextActionValue, iterator->second);
                }
            }
            maxNextActionValue = maxNextActionValue == -std::numeric_limits<float>::max() ? DefaultValue : maxNextActionValue;
        }
        maxNextActionValue = nextState.isTerminal() ? 0.0f : maxNextActionValue;
        ActionValueFunction[pair] += LearningRate * (nextState.getReward() + (DiscountFactor * maxNextActionValue) - ActionValueFunction[pair]);
    }

    void TabularControlRLAgent::updateActionValueES(const State_Action_Pair& pair, const State& nextState, std::vector<Action> actions)
    {
        if (ActionValueFunction.find(pair) == ActionValueFunction.end())
            ActionValueFunction[pair] = { DefaultValue };

        float expectedValue = 0.0f;
        float probability = 1.0f / (actions.size() - 1.0f);
        std::vector<float> summationValues;
        if (!nextState.isTerminal())
        {
            for (Action action : actions)
            {
                State_Action_Pair nextPair = { nextState, action };
                auto iterator = ActionValueFunction.find(nextPair);
                if (iterator != ActionValueFunction.end())
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
        ActionValueFunction[pair] += LearningRate * (nextState.getReward() + (DiscountFactor * expectedValue) - ActionValueFunction[pair]);
    }

    void TabularControlRLAgent::readFunctionFromFile(const char* filename)
    {
        std::ifstream functionFile(filename);
        if (functionFile.is_open())
        {
            std::string line;
            uint8_t counter = 0;

            State_Action_Pair pair;
            while (getline(functionFile, line))
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
                    ActionValueFunction[pair] = std::stof(line);
                    counter = 0;
                    break;
                }
            }
            functionFile.close();
        }
    }

    void TabularControlRLAgent::saveFunctionToFile(const char* filename)
    {
        if (!ActionValueFunction.empty())
        {
            std::ofstream functionFile(filename);
            std::string string;
            for (auto iterator : ActionValueFunction)
            {
                iterator.first.toString(string);
                functionFile << string;
                functionFile << iterator.second << std::endl;
            }
            functionFile.close();
        }
    }
}