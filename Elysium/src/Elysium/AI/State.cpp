#include "State.h"

namespace Elysium
{
    State::State()
    {
    }

    State::State(const std::string& code) :
        m_Code(code)
    {
    }

    State::State(const std::string& code, float reward) :
        m_Code(code),
        m_Reward(reward)
    {
    }

    State::State(const std::string& code, float reward, bool terminal) :
        m_Code(code),
        m_Reward(reward),
        m_Terminal(terminal)
    {
    }
}