#pragma once
#include <Windows.h>
#include <chrono>
#include "CallbackRegistry.h"
#include "AppMessageIDs.h"

namespace HeartControl {

    class EventManager {
    public:
        EventManager(HWND hwnd, EventDispatcher& dispatcher)
            : m_hwnd(hwnd), m_dispatcher(&dispatcher) {
        }

        void initialize(HWND hwnd, EventDispatcher& dispatcher) {
            m_hwnd = hwnd;
            m_dispatcher = &dispatcher;
        }

        // Direct subscription without message passing
        template<typename Func>
        int subscribe(const std::string& eventType, Func&& handler) {
            // Subscribe directly to the dispatcher
            return m_dispatcher->subscribe(eventType, std::forward<Func>(handler));
        }

        // Simple function to unsubscribe
        [[nodiscard]] bool unsubscribe(const std::string& eventType, const int subscriptionId) const
        {
            return m_dispatcher->unsubscribe(eventType, subscriptionId);
        }

        UIEvent retrieveEvent(const int id) {
            return m_eventRegistry.retrieveEvent(id);
        }

        int registerEvent(const UIEvent& evt);

        // Trigger an event (still using message queue for events)
        void triggerEvent(const std::string& eventType, const std::string& target = "",
            const std::string& data = "") {
	        UIEvent evt{
                eventType,
                target,
                data,
                std::chrono::system_clock::now()
            };

            // Post a message to queue the event
            const int eventId = m_eventRegistry.registerEvent(std::move(evt));
            PostMessage(m_hwnd, WM_USER_EVENT, 0, eventId);
        }

    private:
        HWND m_hwnd;
        EventDispatcher* m_dispatcher;
        CallbackRegistry m_eventRegistry;
    };

    inline int EventManager::registerEvent(const UIEvent& evt)
    {
	    return m_eventRegistry.registerEvent(static_cast<UIEvent>(std::move(evt)));
    }
}
