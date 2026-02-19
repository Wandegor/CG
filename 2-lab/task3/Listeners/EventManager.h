#pragma once
#include <unordered_set>

#include "IEventListener.h"

enum class EventType;

class EventManager
{
    std::unordered_map<EventType, std::unordered_set<IEventListener *> > m_listeners;

public:
    void Subscribe(EventType eventType, IEventListener& listener)
    {
        m_listeners[eventType].insert(&listener);
    }

    void Unsubscribe(EventType eventType, IEventListener& listener)
    {
        auto it = m_listeners.find(eventType);
        if (it != m_listeners.end())
        {
            it->second.erase(&listener);
            if (it->second.empty())
            {
                m_listeners.erase(it);
            }
        }
    }

    void NotifyListeners(EventType eventType, const std::optional<sf::Event>& event = std::nullopt)
    {
        if (const auto it = m_listeners.find(eventType); it != m_listeners.end())
        {
            for (auto* listener: it->second)
            {
                listener->Update(eventType, event);
            }
        }
    }
};
