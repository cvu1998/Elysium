#pragma once

#include "Event.h"

#include <sstream>

namespace Elysium
{
    class KeyEvent :public Event
    {

    protected:
        int m_KeyCode;

    protected:
        KeyEvent(int keyCode) : m_KeyCode(keyCode) { }

    public:
        virtual int getCategoryFlags() const override { return EventCategory::EVENT_INPUT | EventCategory::EVENT_KEYBOARD; }
        
        inline int getKeyCode() const { return m_KeyCode; }
    };

    class KeyPressedEvent : public KeyEvent
    {
    private:
        int m_RepeatCount;

    public:
        KeyPressedEvent(int keyCode, int repeatCount) : KeyEvent(keyCode), m_RepeatCount(repeatCount) { }

        static EventType getStaticType() { return EventType::KEY_PRESSED; }
        virtual EventType getEventType() const override { return getStaticType(); }
        virtual const char* getName() const override { return "KEY_PRESSED"; }

        inline int getRepeatCount() const { return m_RepeatCount; }

        #ifdef _DEBUG
        std::string toString() const override
        {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
            return ss.str();
        }
        #endif
    };

    class KeyReleasedEvent : public KeyEvent
    {
    public:
        KeyReleasedEvent(int keyCode) : KeyEvent(keyCode) { }

        static EventType getStaticType() { return EventType::KEY_RELEASED; }
        virtual EventType getEventType() const override { return getStaticType(); }
        virtual const char* getName() const override { return "KEY_RELEASED"; }

        #ifdef _DEBUG
        std::string toString() const override
        {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << m_KeyCode;
            return ss.str();
        }
        #endif
    };
}
