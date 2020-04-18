#pragma once

#include "Event.h"

#include <sstream>

namespace Elysium
{
    class WindowResizeEvent : public Event
    {
    private:
        unsigned int m_Height, m_Width;

    public:
        WindowResizeEvent(unsigned int width, unsigned int height) : m_Width(width), m_Height(height) { }

        static EventType getStaticType() { return EventType::WINDOW_RESIZE; }
        virtual EventType getEventType() const override { return getStaticType(); }
        virtual const char* getName() const override { return "WINDOW_RESIZE"; }
        virtual int getCategoryFlags() const override { return EventCategory::EVENT_APPLICATION; }

        inline unsigned int getHeight() const { return m_Height; }
        inline unsigned int getWidth() const { return m_Width; }

        #ifdef _DEBUG
        std::string toString() const override
        {
            std::stringstream ss;
            ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
            return ss.str();
        }
        #endif  
    };

    class WindowCloseEvent : public Event
    {
    public:
        WindowCloseEvent() { }

        static EventType getStaticType() { return EventType::WINDOWS_CLOSE; }
        virtual EventType getEventType() const override { return getStaticType(); }
        virtual const char* getName() const override { return "WINDOWS_CLOSE"; }
        virtual int getCategoryFlags() const override { return EventCategory::EVENT_APPLICATION; }

        #ifdef _DEBUG
        std::string toString() const override
        {
            std::stringstream ss;
            ss << "WindowCloseEvent";
            return ss.str();
        }
        #endif  
    };

    class AppRenderEvent : public Event
    {
    public:
        AppRenderEvent() { }

        static EventType getStaticType() { return EventType::APP_RENDER; }
        virtual EventType getEventType() const override { return getStaticType(); }
        virtual const char* getName() const override { return "APP_RENDER"; }
        virtual int getCategoryFlags() const override { return EventCategory::EVENT_APPLICATION; }
    };

    class AppTickEvent : public Event
    {
    public:
        AppTickEvent() { }

        static EventType getStaticType() { return EventType::APP_TICK; }
        virtual EventType getEventType() const override { return getStaticType(); }
        virtual const char* getName() const override { return "APP_TICK"; }
        virtual int getCategoryFlags() const override { return EventCategory::EVENT_APPLICATION; }
    };


    class AppUpdateEvent : public Event
    {
    public:
        AppUpdateEvent() { }

        static EventType getStaticType() { return EventType::APP_UPDATE; }
        virtual EventType getEventType() const override { return getStaticType(); }
        virtual const char* getName() const override { return "APP_UPDATE"; }
        virtual int getCategoryFlags() const override { return EventCategory::EVENT_APPLICATION; }
    };
}
