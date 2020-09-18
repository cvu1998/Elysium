#pragma once

#include <sstream>

namespace Elysium
{
    class State
    {
    private:
        std::string m_Code;
        float m_Reward = 0.0f;
        bool m_Terminal = false;

    public:
        State();
        State(const std::string& code);
        State(const std::string& code, float reward);
        State(const std::string& code, float reward, bool terminal);
        State(float reward, bool terminal);

        inline const char* getCode() const { return m_Code.c_str(); }
        inline float getReward() const { return m_Reward; }
        inline bool isTerminal() const { return m_Terminal; }

        bool operator==(const State& state) const;

    };

    using Action = unsigned int;

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
            return ((0 == strcmp(this->State.getCode(), actionPair.State.getCode())) &&
                this->Action == actionPair.Action);
        }
    };
}