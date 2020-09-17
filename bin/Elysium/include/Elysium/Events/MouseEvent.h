#pragma once

#include "Event.h"

#include <sstream>

namespace Elysium
{
    class MouseMovedEvent : public Event
    {
    private:
        float m_MouseX, m_MouseY;

    public:
        MouseMovedEvent(float x, float y) : m_MouseX(x), m_MouseY(y) { }

        static EventType getStaticType() { return EventType::MOUSE_MOVED; }
        virtual EventType getEventType() const override { return getStaticType(); }
        virtual const char* getName() const override { return "MOUSE_MOVED"; }
        virtual int getCategoryFlags() const override { return EventCategory::EVENT_INPUT | EventCategory::EVENT_MOUSE; }

        inline float getX() const { return m_MouseX; }
        inline float getY() const { return m_MouseY; }
        
        #ifdef _DEBUG
        std::string toString() const override
        {
            std::stringstream ss;
            ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
            return ss.str();
        }
        #endif          
    };

    class MouseScrollEvent : public Event
    {
    private:
        float m_XOffset, m_YOffset;

    public:
        MouseScrollEvent(float x, float y) : m_XOffset(x), m_YOffset(y) { }

        static EventType getStaticType() { return EventType::MOUSE_SCROLLED; }
        virtual EventType getEventType() const override { return getStaticType(); }
        virtual const char* getName() const override { return "MOUSE_SCROLLED"; }
        virtual int getCategoryFlags() const override { return EventCategory::EVENT_INPUT | EventCategory::EVENT_MOUSE; }

        inline float getXOffset() const { return m_XOffset; }
        inline float getYOffset() const { return m_YOffset; }

        #ifdef _DEBUG
        std::string toString() const override
        {
            std::stringstream ss;
            ss << "MouseScrollEvent: " << m_XOffset << ", " << m_YOffset;
            return ss.str();
        }
        #endif          
    };

    class MouseButtonEvent : public Event
    {
    protected:
        int m_Button;

    protected:
        MouseButtonEvent(int button) : m_Button(button) { }

    public:
        virtual int getCategoryFlags() const override { return EventCategory::EVENT_INPUT | EventCategory::EVENT_MOUSE; }

        inline int getMouseButton() const { return m_Button; }
    };

    class MouseButtonPressedEvent : public MouseButtonEvent
    {
    public:
        MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

        static EventType getStaticType() { return EventType::MOUSE_BUTTON_PRESSED; }
        virtual EventType getEventType() const override { return getStaticType(); }
        virtual const char* getName() const override { return "MOUSE_BUTTON_PRESSED"; }

        #ifdef _DEBUG
        std::string toString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonPressedEvent: " << m_Button;
            return ss.str();
        }
        #endif
    };

    class MouseButtonReleasedEvent : public MouseButtonEvent
    {
    public:
        MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

        static EventType getStaticType() { return EventType::MOUSE_BUTTON_RELEASED; }
        virtual EventType getEventType() const override { return getStaticType(); }
        virtual const char* getName() const override { return "MOUSE_BUTTON_RELEASED"; }

        #ifdef _DEBUG
        std::string toString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonReleasedEvent: " << m_Button;
            return ss.str();
        }
        #endif
    };
}
