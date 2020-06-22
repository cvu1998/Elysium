#pragma once

#include <functional>
#include <iostream>
#include <string>

#include "Elysium/Utility.h"

namespace Elysium
{
    enum class EventType
    {
        NONE = 0,

        WINDOWS_CLOSE,
        WINDOWS_FOCUS,
        WINDOW_LOST_FOCUS,
        WINDOW_MOVED,
        WINDOW_RESIZE,

        APP_RENDER,
        APP_TICK,
        APP_UPDATE,

        KEY_PRESSED,
        KEY_RELEASED,

        MOUSE_BUTTON_PRESSED,
        MOUSE_BUTTON_RELEASED,
        MOUSE_MOVED,
        MOUSE_SCROLLED
    };

    enum EventCategory
    {
        NONE = 0,
        EVENT_APPLICATION   = BIT(0),
        EVENT_INPUT         = BIT(1),
        EVENT_KEYBOARD      = BIT(2),
        EVENT_MOUSE         = BIT(3),
        EVENT_MOUSE_SCROLL  = BIT(4)
    };

    class Event
    {
        friend class EventDispatcher;
    public:
        bool Handled = false;

    public:
        virtual EventType getEventType() const = 0;
        virtual int getCategoryFlags() const = 0;
        virtual const char* getName() const = 0;

        #ifdef _DEBUG
        virtual std::string toString() const { return getName(); }
        friend inline std::ostream& operator<<(std::ostream& os, const Event& e)
        {
            return os << e.toString();
        }
        #endif

        inline bool isInCategory(EventCategory category) { return getCategoryFlags() & category; }
    };

    class EventDispatcher
    {
        template<typename T>
        using EventFunction = std::function<bool(T&)>;

    private:
        Event& m_Event;
    public:
        EventDispatcher(Event& event) : m_Event(event) { }
        
        template<typename T>
        bool Dispatch(EventFunction<T> function)
        {
            if (m_Event.getEventType() == T::getStaticType())
            {
                m_Event.Handled = function(*(T*)&m_Event);
                return true;
            }
            return false;
        }
    };
}