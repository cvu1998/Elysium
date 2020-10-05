#pragma once

#include <unordered_map>

#include "RLAgent.h"

namespace Elysium
{
    class TabularRLAgent : public RLAgent
    {
    protected:
        struct hash_state 
        {
            size_t operator()(const State& state) const
            {
                std::string string(state.getCode());
                return std::hash<std::string>{}(string);
            }
        };

    public:
        std::unordered_map<State, float, hash_state> StateValueFunction;

    public:
        TabularRLAgent(float learningRate, float discountFactor, float defaultValue);

        float getStateValue(const State& state);
        void updateStateValue(const State& currentState, const State& nextState);

        bool readFunctionFromFile(const char* filename);
        void saveFunctionToFile(const char* filename);

    };

    class TabularControlRLAgent : public RLAgent
    {
    private:
        struct hash_pair 
        {
            size_t operator()(const State_Action_Pair& pair) const
            {
                std::string string(pair.State.getCode() + std::to_string((unsigned int)pair.Action));
                return std::hash<std::string>{}(string);
            }
        };

    public:
        std::unordered_map<State_Action_Pair, float, hash_pair> ActionValueFunction;

    public:
        TabularControlRLAgent(float learningRate, float discountFactor, float defaultValue);

        float getStateActionPairValue(const State_Action_Pair& pair);
        void updateActionValueQL(const State_Action_Pair& pair, const State& nextState, std::vector<Action> actions);
        void updateActionValueES(const State_Action_Pair& pair, const State& nextState, std::vector<Action> actions);

        bool readFunctionFromFile(const char* filename);
        void saveFunctionToFile(const char* filename);
    };
}