#pragma once

#include <sstream>

namespace Elysium
{
    class State
    {
    private:
        size_t m_Code;
        float m_Reward = 0.0f;
        bool m_Terminal = false;

    public:
        State(size_t code);
        State(size_t code, float reward);
        State(size_t code, float reward, bool terminal);
        State(float reward, bool terminal);

        inline size_t getCode() const { return m_Code; }
        inline float getReward() const { return m_Reward; }
        inline bool isTerminal() const { return m_Terminal; }

        bool operator==(const State& state) const;

    };

    using Action = size_t;

    struct State_Action_Pair
    {
        State State;
        Action Action = -1;

        void toString(std::string& string) const
        {
            std::stringstream ss;
            ss << State.getCode() << '\n' << (unsigned int)Action << '\n';
            string = ss.str();
        }

        bool operator==(const State_Action_Pair& actionPair) const
        {
            return (this->State.getCode() == actionPair.State.getCode() &&
                this->Action == actionPair.Action);
        }
    };
}