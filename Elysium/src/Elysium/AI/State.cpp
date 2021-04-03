#include "State.h"

namespace Elysium
{
    State::State(size_t code) :
        m_Code(code)
    {
    }

    State::State(size_t code, float reward) :
        m_Code(code),
        m_Reward(reward)
    {
    }

    State::State(size_t code, float reward, bool terminal) :
        m_Code(code),
        m_Reward(reward),
        m_Terminal(terminal)
    {
    }

    State::State(float reward, bool terminal) :
        m_Reward(reward),
        m_Terminal(terminal)
    {
    }

    bool State::operator==(const State& state) const
    {
        return (this->getCode() == state.getCode());
    }
}